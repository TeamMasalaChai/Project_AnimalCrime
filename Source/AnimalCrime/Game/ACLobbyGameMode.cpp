
#include "ACLobbyGameMode.h"
#include "ACLobbyPlayerController.h"
#include "ACLobbyGameState.h"
#include "Character/ACLobbyCharacter.h"
#include "Character/ACMafiaCharacter.h"
#include "ACAdvancedFriendsGameInstance.h"
#include "AnimalCrime.h"

AACLobbyGameMode::AACLobbyGameMode()
{
	PlayerControllerClass = AACLobbyPlayerController::StaticClass();
	GameStateClass = AACLobbyGameState::StaticClass();

	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnBP(
		TEXT("/Game/Project/Character/BP_VoiceTestCharacter")
	);

	if (DefaultPawnBP.Succeeded())
	{
		DefaultPawnClass = DefaultPawnBP.Class;
	}

	bUseSeamlessTravel = true;
	//DefaultPawnClass = AACLobbyCharacter::StaticClass();
}
void AACLobbyGameMode::StartGamePlay()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	//World->ServerTravel("/Game/Project/Map/DemoMap?listen", false);

	UACAdvancedFriendsGameInstance* GI = GetGameInstance<UACAdvancedFriendsGameInstance>();
	if (GI == nullptr)
	{
		return;
	}
	GI->LoadGameMap();


	AC_LOG(LogSY, Log, TEXT("맵이동"));	
}



