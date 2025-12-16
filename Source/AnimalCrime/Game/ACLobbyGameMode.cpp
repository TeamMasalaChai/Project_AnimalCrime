
#include "ACLobbyGameMode.h"

#include "ACAdvancedFriendsGameInstance.h"
#include "ACLobbyPlayerController.h"
#include "Character/ACTestMafiaCharacter.h"

AACLobbyGameMode::AACLobbyGameMode()
{
	PlayerControllerClass = AACLobbyPlayerController::StaticClass();

    static ConstructorHelpers::FClassFinder<APawn> DefaultPawnBP(
        TEXT("/Game/Project/Character/BP_VoiceTestCharacter")
    );

    if (DefaultPawnBP.Succeeded())
    {
        DefaultPawnClass = DefaultPawnBP.Class;
    }
	bUseSeamlessTravel = true;
	//DefaultPawnClass = AACTestMafiaCharacter::StaticClass();
}

void AACLobbyGameMode::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        GetWorld()->GetTimerManager().SetTimer(
            TravelTimerHandle,
            this,
            &AACLobbyGameMode::StartGameTravel,
            15.0f,
            false
        );
    }
}

void AACLobbyGameMode::StartGameTravel()
{
    GetWorld()->ServerTravel("/Game/Project/Map/henaMap", true);
}