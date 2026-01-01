
#include "ACPlayerControllerBase.h"
#include "ACAdvancedFriendsGameInstance.h"
#include "AudioDevice.h"
#include "AnimalCrime.h"

void AACPlayerControllerBase::Client_CleanupVoiceBeforeTravel_Implementation()
{
	AC_LOG(LogSY, Log, TEXT("Client_CleanupVoiceBeforeTravel!!"));

	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		AC_LOG(LogSY, Warning, TEXT("World is nullptr"));
		return;
	}

	// PIE에서는 Voice / Audio 정리 스킵
	if (World->IsPlayInEditor())
	{
		AC_LOG(LogSY, Log, TEXT("PIE detected - skip voice cleanup"));
		Server_NotifyVoiceCleaned();
		return;
	}

	IOnlineVoicePtr Voice = Online::GetVoiceInterface();
	if (Voice.IsValid() == true)
	{
		Voice->RemoveAllRemoteTalkers();
		Voice->StopNetworkedVoice(0);
		Voice->UnregisterLocalTalker(0);
		AC_LOG(LogSY, Log, TEXT("Voice cleanup completed"));
	}
	else
	{
		AC_LOG(LogSY, Warning, TEXT("Voice is fail"));
	}

	// AudioDevice 정리
	if (FAudioDeviceHandle AudioDeviceHandle = World->GetAudioDevice())
	{
		// StopAllSounds는 게임 스레드에서 안전
		AudioDeviceHandle->StopAllSounds(true);
		// Flush로 오디오 처리 대기
		AudioDeviceHandle->Flush(World);
	}

	// Polling 시작
	ClientPollingAttempts = 0;
	CheckClientVoiceCleanupComplete();

	// 약간의 딜레이를 주고 서버에 알림
	//FTimerHandle CleanupTimer;
	//GetWorldTimerManager().SetTimer(CleanupTimer, [this]()
	//	{
	//		Server_NotifyVoiceCleaned();
	//	}, 0.15f, false);  // 150ms 대기

	//Server_NotifyVoiceCleaned();
}

void AACPlayerControllerBase::Server_NotifyVoiceCleaned_Implementation()
{
	UACAdvancedFriendsGameInstance* GI = GetGameInstance<UACAdvancedFriendsGameInstance>();
	if (GI == nullptr)
	{
		AC_LOG(LogSY, Error, TEXT("GI is nullptr"));
		return;
	}
	AC_LOG(LogSY, Log, TEXT("Server_NotifyVoiceCleaned!!"));
	GI->OnClientVoiceCleanupFinished();
}

void AACPlayerControllerBase::CheckClientVoiceCleanupComplete()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        AC_LOG(LogSY, Error, TEXT("World is null in polling"));
        Server_NotifyVoiceCleaned();
        return;
    }

    // VoIP 컴포넌트 존재 여부 확인
    bool bVoipComponentsExist = false;
    int32 RegisteredCount = 0;

    for (TObjectIterator<UAudioComponent> It; It; ++It)
    {
        UAudioComponent* AudioComp = *It;

        // 이 World의 컴포넌트만 체크
        if (AudioComp->GetWorld() != World)
            continue;

        // VoIP 관련 컴포넌트인지 확인
        FString CompName = AudioComp->GetName();
        if (CompName.Contains(TEXT("VoipListener")) ||
            CompName.Contains(TEXT("VoiceCap")))
        {
            if (AudioComp->IsRegistered())
            {
                bVoipComponentsExist = true;
                RegisteredCount++;
                AC_LOG(LogSY, Log, TEXT("Polling: VoIP component still registered: %s"), *CompName);
            }
        }
    }

    // 정리 완료 확인
    if (!bVoipComponentsExist)
    {
        // ✅ 정리 완료!
        AC_LOG(LogSY, Log, TEXT("✅ Client voice cleanup verified! (Attempts: %d)"),
            ClientPollingAttempts);
        Server_NotifyVoiceCleaned();
        return;
    }

    // 아직 정리 중
    ClientPollingAttempts++;

    AC_LOG(LogSY, Log, TEXT("⏳ Still cleaning... (Attempt %d/%d, Components: %d)"),
        ClientPollingAttempts, MaxClientPollingAttempts, RegisteredCount);

    // 타임아웃 체크
    if (ClientPollingAttempts >= MaxClientPollingAttempts)
    {
        AC_LOG(LogSY, Warning, TEXT("⚠️ Client voice cleanup timeout! Proceeding anyway. (Remaining components: %d)"),
            RegisteredCount);
        Server_NotifyVoiceCleaned();
        return;
    }

    // 다시 체크
    FTimerHandle RetryTimer;
    GetWorldTimerManager().SetTimer(RetryTimer,
        this, &AACPlayerControllerBase::CheckClientVoiceCleanupComplete,
        ClientPollingInterval, false);
}
