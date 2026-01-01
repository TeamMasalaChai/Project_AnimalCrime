
#include "ACAdvancedFriendsGameInstance.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Kismet/GameplayStatics.h"
#include "AdvancedSessionsLibrary.h"
#include "CreateSessionCallbackProxyAdvanced.h"
#include "Engine/NetDriver.h"
#include "Engine/Engine.h"
#include "AudioDevice.h"
#include "AnimalCrime.h"
#include "Net/VoiceConfig.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"
#include "ACPlayerControllerBase.h"
#include "MoviePlayer.h"
#include "Blueprint/UserWidget.h"

#pragma region 엔진 제공 함수
void UACAdvancedFriendsGameInstance::Init()
{
	Super::Init();

	// 네트워크 끊김 처리
	GetEngine()->OnNetworkFailure().AddUObject(this, &UACAdvancedFriendsGameInstance::HandleNetworkFailure);
	GetEngine()->OnTravelFailure().AddUObject(this, &UACAdvancedFriendsGameInstance::HandleTravelFailure);

	// 세션이 Destroy 되었을 때 호출될 델리게이트 등록
	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (SessionInterface.IsValid() == true)
	{
		OnDestroySessionCompleteHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(
			FOnDestroySessionCompleteDelegate::CreateUObject(this, &UACAdvancedFriendsGameInstance::OnDestroySessionComplete));
	}

	// 월드가 파괴되거나 변경될 때 호출되는 델리게이트 등록
	//FWorldDelegates::OnWorldCleanup.AddUObject(this, &UACAdvancedFriendsGameInstance::OnWorldCleanup);

	// 맵 관련 초기화
	CurrentMapType = EMapType::None;

	LobbyMapName = TEXT("LobbyMap");
	GameMapName = TEXT("henaMap");

	// PIE(에디터 플레이) 환경에서도 SeamlessTravel 허용
	UE_LOG(LogTemp, Warning, TEXT("UACMainGameInstance::Init"));
	if (GEngine)
	{
		GEngine->Exec(GetWorld(), TEXT("net.AllowPIESeamlessTravel 1"));
	}

}

void UACAdvancedFriendsGameInstance::Shutdown()
{
	// Init에서 등록한 DestroySession 델리게이트 해제
	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (SessionInterface.IsValid() && OnDestroySessionCompleteHandle.IsValid())
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteHandle);
	}

	UE_LOG(LogSY, Warning, TEXT("UACMainGameInstance::Shutdown"));

	Super::Shutdown();
}

void UACAdvancedFriendsGameInstance::OnStart()
{
	Super::OnStart();

	UE_LOG(LogSY, Warning, TEXT("UACMainGameInstance::OnStart"));
}
#pragma endregion

#pragma region Map Level
void UACAdvancedFriendsGameInstance::LoadLobbyMap()
{
	UpdateMap(EMapType::Lobby);
}

void UACAdvancedFriendsGameInstance::LoadGameMap()
{
	UpdateMap(EMapType::Game);
}

EMapType UACAdvancedFriendsGameInstance::GetMapType() const
{
	return CurrentMapType;
}

void UACAdvancedFriendsGameInstance::UpdateMap(const EMapType InMapType)
{
	if (CurrentMapType == InMapType)
	{
		UE_LOG(LogTemp, Error, TEXT("CurrentMapType == InMapType"));
		return;
	}

	// 현재 맵을 변경된 맵으로 Update
	CurrentMapType = InMapType;
	NumClientsReady = 0;
	bServerVoiceCleaned = false;

	// Client들에게 먼저 Voice 정리 요청
	// Client들이 먼저 자기 Voice를 끊어야 Server에 새 패킷이 안 들어옴
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AACPlayerControllerBase* PC = Cast<AACPlayerControllerBase>(It->Get());
		if (PC == nullptr)
		{
			continue;
		}
		PC->Client_CleanupVoiceBeforeTravel();
	}

	//// Server는  약간 늦게 Voice 정리
	//if (GetWorld()->GetNetMode() != NM_Client)
	//{
	//	// 100ms 후에 Server Voice 정리
	//	// → Client들의 Voice 송신이 먼저 중단될 시간
	//	FTimerHandle ServerCleanupTimer;
	//	GetWorld()->GetTimerManager().SetTimer(ServerCleanupTimer, [this]()
	//		{
	//			IOnlineVoicePtr Voice = Online::GetVoiceInterface();
	//			if (Voice.IsValid())
	//			{
	//				// 이제 안전하게 RemoteTalker 제거 가능
	//				// Client들이 이미 송신 중단했으므로 새 패킷 안 들어옴
	//				Voice->RemoveAllRemoteTalkers();
	//				Voice->StopNetworkedVoice(0);
	//				Voice->UnregisterLocalTalker(0);
	//				UE_LOG(LogSY, Log, TEXT("Server voice cleaned (delayed)"));
	//			}

	//			// Audio도 정리
	//			if (UWorld* World = GetWorld())
	//			{
	//				if (FAudioDeviceHandle AudioDeviceHandle = World->GetAudioDevice())
	//				{
	//					AudioDeviceHandle->StopAllSounds(true);
	//					AudioDeviceHandle->Flush(World);
	//				}
	//			}

	//			bVoiceInitialized = false;
	//		}, 0.1f, false);  // 100ms 딜레이
	//}
}
#pragma endregion

void UACAdvancedFriendsGameInstance::HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	UE_LOG(LogSY, Warning, TEXT("Network Failure: %s"), *ErrorString);

	bVoiceInitialized = false;

	// 세션 정리
	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(World);
	if (SessionInterface.IsValid() == true)
	{
		SessionInterface->DestroySession(NAME_GameSession);
	}
}

void UACAdvancedFriendsGameInstance::HandleTravelFailure(UWorld* World, ETravelFailure::Type FailureType, const FString& ErrorString)
{
	UE_LOG(LogSY, Warning, TEXT("Travel Failure: %s"), *ErrorString);

	bVoiceInitialized = false;

	// 세션 정리
	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(World);
	if (SessionInterface.IsValid() == true)
	{
		SessionInterface->DestroySession(NAME_GameSession);
	}
}

void UACAdvancedFriendsGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	bVoiceInitialized = false;
}

void UACAdvancedFriendsGameInstance::CheckServerVoiceCleanup()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogSY, Error, TEXT("World is null in server polling"));
		bServerVoiceCleaned = true;
		CheckFinalStateBeforeTravel();
		return;
	}

	// Server의 VoIP 컴포넌트 확인
	bool bServerVoipExists = false;
	int32 RegisteredCount = 0;

	for (TObjectIterator<UAudioComponent> It; It; ++It)
	{
		UAudioComponent* AudioComp = *It;

		if (AudioComp->GetWorld() != World)
			continue;

		FString CompName = AudioComp->GetName();
		if (CompName.Contains(TEXT("VoipListener")) ||
			CompName.Contains(TEXT("VoiceCap")))
		{
			if (AudioComp->IsRegistered())
			{
				bServerVoipExists = true;
				RegisteredCount++;
			}
		}
	}

	// Server 정리 완료 확인
	if (!bServerVoipExists)
	{
		UE_LOG(LogSY, Log, TEXT("✅ Server voice cleanup verified! (Attempts: %d)"),
			ServerCleanupPollingAttempts);
		bServerVoiceCleaned = true;

		// Step 4: 최종 상태 확인 시작
		CheckFinalStateBeforeTravel();
		return;
	}

	// 아직 정리 중
	ServerCleanupPollingAttempts++;

	if (ServerCleanupPollingAttempts % 10 == 0) // 100ms마다 로그
	{
		UE_LOG(LogSY, Log, TEXT("⏳ Server still cleaning... (Attempt %d/%d, Components: %d)"),
			ServerCleanupPollingAttempts, MaxServerCleanupPollingAttempts, RegisteredCount);
	}

	// 타임아웃 체크
	if (ServerCleanupPollingAttempts >= MaxServerCleanupPollingAttempts)
	{
		UE_LOG(LogSY, Warning, TEXT("⚠️ Server voice cleanup timeout! Proceeding anyway. (Remaining: %d)"),
			RegisteredCount);
		bServerVoiceCleaned = true;
		CheckFinalStateBeforeTravel();
		return;
	}

	// 다시 체크
	FTimerHandle RetryTimer;
	World->GetTimerManager().SetTimer(RetryTimer,
		this, &UACAdvancedFriendsGameInstance::CheckServerVoiceCleanup,
		ServerPollingInterval, false);
}

void UACAdvancedFriendsGameInstance::CheckFinalStateBeforeTravel()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogSY, Error, TEXT("World is null in final check"));
		DoServerTravel();
		return;
	}

	// 최종 검증: World에 남은 VoIP 컴포넌트 확인
	bool bNoVoipComponents = true;
	int32 RemainingComponents = 0;

	for (TObjectIterator<UAudioComponent> It; It; ++It)
	{
		UAudioComponent* AudioComp = *It;

		if (AudioComp->GetWorld() != World)
			continue;

		FString CompName = AudioComp->GetName();
		if (CompName.Contains(TEXT("VoipListener")) ||
			CompName.Contains(TEXT("VoiceCap")))
		{
			if (AudioComp->IsRegistered())
			{
				bNoVoipComponents = false;
				RemainingComponents++;
			}
		}
	}

	// 모든 조건 확인
	if (bNoVoipComponents)
	{
		UE_LOG(LogSY, Log, TEXT("✅ Final check passed! No VoIP components remaining (Attempts: %d)"),
			FinalCheckPollingAttempts);
		DoServerTravel();
		return;
	}

	// 아직 컴포넌트가 남아있음
	FinalCheckPollingAttempts++;

	if (FinalCheckPollingAttempts % 10 == 0) // 100ms마다 로그
	{
		UE_LOG(LogSY, Log, TEXT("⏳ Final check: %d VoIP components still exist (Attempt %d/%d)"),
			RemainingComponents, FinalCheckPollingAttempts, MaxFinalCheckPollingAttempts);
	}

	// 타임아웃 체크
	if (FinalCheckPollingAttempts >= MaxFinalCheckPollingAttempts)
	{
		UE_LOG(LogSY, Warning, TEXT("⚠️ Final check timeout! Forcing travel with %d components remaining"),
			RemainingComponents);
		DoServerTravel();
		return;
	}

	// 다시 체크
	FTimerHandle RetryTimer;
	World->GetTimerManager().SetTimer(RetryTimer,
		this, &UACAdvancedFriendsGameInstance::CheckFinalStateBeforeTravel,
		ServerPollingInterval, false);
}

//void UACAdvancedFriendsGameInstance::OnWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources)
//{
//	// 현재 작동 중인 월드가 정리될 때 실행됨 (서버/클라이언트 공통)
//	if (World == GetWorld())
//	{
//		UE_LOG(LogSY, Warning, TEXT("World Cleanup Detected - Resetting Voice State"));
//
//		// Voice 정리
//		CleanupVoiceSystem();
//	}
//}

//void UACAdvancedFriendsGameInstance::CleanupVoiceSystem()
//{
//	if (!bVoiceInitialized)
//	{
//		return;
//	}
//
//	IOnlineVoicePtr Voice = Online::GetVoiceInterface();
//	if (Voice.IsValid())
//	{
//		UE_LOG(LogSY, Warning, TEXT("Cleaning up Voice System"));
//
//		// 1. 모든 원격 Talker 먼저 제거 (중요!)
//		Voice->RemoveAllRemoteTalkers();
//
//		// 2. 네트워크 보이스 중지
//		Voice->StopNetworkedVoice(0);
//
//		// 3. 로컬 Talker 제거
//		Voice->UnregisterLocalTalker(0);
//
//		bVoiceInitialized = false;
//
//		UE_LOG(LogSY, Warning, TEXT("Voice System Cleaned"));
//	}
//}

void UACAdvancedFriendsGameInstance::TryStartVoice()
{
	UE_LOG(LogSY, Log, TEXT("Try Start Voice"));
	//만약 이미 보이스를 연결했다면 return.
	if (bVoiceInitialized == true)
	{
		UE_LOG(LogSY, Log, TEXT("Voice가 이미 연결되어있음"));
		return;
	}

	IOnlineVoicePtr Voice = Online::GetVoiceInterface();
	if (Voice.IsValid() == false)
	{
		UE_LOG(LogSY, Log, TEXT("Voice: VoiceInterface 없음"));
		return;
	}

	IOnlineSessionPtr Session = Online::GetSessionInterface(GetWorld());
	if (Session.IsValid() == false || Session->GetNamedSession(NAME_GameSession) == nullptr)
	{
		UE_LOG(LogSY, Log, TEXT("Voice: 세션 없음, Voice 시작 안 함"));
		return;
	}

	// Server의 경우 Remote Talker 등록
	//if (GetWorld()->GetNetMode() != NM_Client)
	//{
	//	UE_LOG(LogSY, Log, TEXT("--- Registering Remote Talkers ---"));
	//	int32 Count = 0;

	//	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	//	{
	//		APlayerController* PC = It->Get();
	//		if (PC && !PC->GetLocalPlayer() && PC->PlayerState)
	//		{
	//			FUniqueNetIdRepl UniqueId = PC->PlayerState->GetUniqueId();
	//			if (UniqueId.IsValid())
	//			{
	//				Voice->RegisterRemoteTalker(*UniqueId);
	//				UE_LOG(LogSY, Log, TEXT("  %s"), *PC->GetName());
	//				Count++;
	//			}
	//		}
	//	}

	//	UE_LOG(LogSY, Log, TEXT("  Total: %d"), Count);
	//}

	//보이스 연결
	Voice->RegisterLocalTalker(0);
	Voice->StartNetworkedVoice(0);
	bVoiceInitialized = true;
	UE_LOG(LogSY, Log, TEXT("Voice initialized"));
}

void UACAdvancedFriendsGameInstance::OnClientVoiceCleanupFinished()
{
	UE_LOG(LogSY, Log, TEXT("OnClientVoiceCleanupFinished!!"));
	++NumClientsReady;

	// 모든 Client가 정리를 완료했는가?
	if (NumClientsReady >= GetWorld()->GetNumPlayerControllers())
	{
		UE_LOG(LogSY, Log, TEXT("=== ✅ All clients finished cleanup ==="));

		// Step 2: 이제 Server Voice 정리 시작!
		// Client들이 이미 송신을 중단했으므로 안전하게 RemoteTalker 제거 가능

		if (GetWorld()->GetNetMode() != NM_Client)
		{
			UE_LOG(LogSY, Log, TEXT("--- Server: Starting voice cleanup ---"));

			IOnlineVoicePtr Voice = Online::GetVoiceInterface();
			if (Voice.IsValid())
			{
				Voice->RemoveAllRemoteTalkers();
				Voice->StopNetworkedVoice(0);
				Voice->UnregisterLocalTalker(0);
				UE_LOG(LogSY, Log, TEXT("Server voice cleanup requested"));
			}

			// Audio 정리
			if (UWorld* World = GetWorld())
			{
				if (FAudioDeviceHandle AudioDeviceHandle = World->GetAudioDevice())
				{
					AudioDeviceHandle->StopAllSounds(true);
					AudioDeviceHandle->Flush(World);
					UE_LOG(LogSY, Log, TEXT("Server audio flush requested"));
				}
			}

			bVoiceInitialized = false;

			// Step 3: Server Voice 정리 완료 확인 시작
			ServerCleanupPollingAttempts = 0;
			CheckServerVoiceCleanup();
		}
		else
		{
			// Pure Client는 Server 정리 스킵
			bServerVoiceCleaned = true;
			CheckFinalStateBeforeTravel();
		}
	}
}

void UACAdvancedFriendsGameInstance::DoServerTravel()
{
	UE_LOG(LogSY, Log, TEXT("DoServerTravel!!"));
	bVoiceInitialized = false;

	UWorld* World = GetWorld();
	if (World)
	{
		// 최종 안전 체크: 남은 VoIP 컴포넌트 강제 정리
		int32 ForcedRemovalCount = 0;
		TArray<UAudioComponent*> ComponentsToRemove;

		for (TObjectIterator<UAudioComponent> It; It; ++It)
		{
			UAudioComponent* AudioComp = *It;

			if (AudioComp->GetWorld() != World)
				continue;

			FString CompName = AudioComp->GetName();
			if (CompName.Contains(TEXT("VoipListener")) ||
				CompName.Contains(TEXT("VoiceCap")))
			{
				if (AudioComp->IsRegistered())
				{
					UE_LOG(LogSY, Warning, TEXT("⚠️ Found lingering component: %s"), *CompName);
					ComponentsToRemove.Add(AudioComp);
				}
			}
		}

		// 강제 제거
		for (UAudioComponent* Comp : ComponentsToRemove)
		{
			Comp->UnregisterComponent();
			ForcedRemovalCount++;
		}

		if (ForcedRemovalCount > 0)
		{
			UE_LOG(LogSY, Warning, TEXT("🔧 Force removed %d VoIP components"), ForcedRemovalCount);

			// Audio Flush
			if (FAudioDeviceHandle AudioDeviceHandle = World->GetAudioDevice())
			{
				AudioDeviceHandle->Flush(World);
			}
		}
		else
		{
			UE_LOG(LogSY, Log, TEXT("✅ No lingering VoIP components - safe to travel"));
		}
	}

	switch (CurrentMapType)
	{
	case EMapType::Lobby:
		GetWorld()->ServerTravel("LobbyMap", true);
		break;
	case EMapType::Game:
		GetWorld()->ServerTravel("DemoMap", true);
		break;
	}
}
