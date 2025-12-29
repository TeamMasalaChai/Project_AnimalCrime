
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
		Voice->StopNetworkedVoice(0);
		Voice->UnregisterLocalTalker(0);
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
	}

	//FTimerHandle CleanupTimer;
	//GetWorldTimerManager().SetTimer(CleanupTimer, [this]()
	//	{
	//		Server_NotifyVoiceCleaned();
	//	}, 0.1f, false);

	Server_NotifyVoiceCleaned();
}

void AACPlayerControllerBase::Server_NotifyVoiceCleaned_Implementation()
{
	UACAdvancedFriendsGameInstance* GI = GetGameInstance<UACAdvancedFriendsGameInstance>();
	if (GI == nullptr)
	{
		AC_LOG(LogSY, Error, TEXT("GI is nullptr"));
		return;
	}
	GI->OnClientVoiceCleanupFinished();
	AC_LOG(LogSY, Log, TEXT("Server_NotifyVoiceCleaned!!"));
}