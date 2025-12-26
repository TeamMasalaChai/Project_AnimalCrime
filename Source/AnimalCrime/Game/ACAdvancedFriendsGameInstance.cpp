
#include "ACAdvancedFriendsGameInstance.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Kismet/GameplayStatics.h"
#include "AdvancedSessionsLibrary.h"
#include "CreateSessionCallbackProxyAdvanced.h"
#include "Engine/NetDriver.h"
#include "Engine/Engine.h"

#include "AnimalCrime.h"

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
        return ;
    }

    // 현재 맵을 변경된 맵으로 Update
    CurrentMapType = InMapType;

    switch (InMapType)
    {
    case EMapType::Lobby:
        GetWorld()->ServerTravel("LobbyMap", true);
        break;
    case EMapType::Game:
        GetWorld()->ServerTravel("DemoMap", true);
        break;
    default:
        break;
    }
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

void UACAdvancedFriendsGameInstance::TryStartVoice()
{
    UE_LOG(LogSY, Log, TEXT("Try Start Voice"));
    //만약 이미 보이스를 연결했다면 return.
    if (bVoiceInitialized == true)
    {
        return;
    }

    IOnlineVoicePtr Voice = Online::GetVoiceInterface();
    if (Voice.IsValid() == false)
    {
        UE_LOG(LogSY, Log, TEXT("Voice가 유효하지 않음"));
        return;
    }

    //보이스 연결
    Voice->StartNetworkedVoice(0);
    bVoiceInitialized = true;
    UE_LOG(LogSY, Log, TEXT("Voice initialized"));
}
