
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

	//DefaultPawnClass = AACTestMafiaCharacter::StaticClass();
}

void AACLobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([&]()
		{
		UACAdvancedFriendsGameInstance* Instance = GetGameInstance<UACAdvancedFriendsGameInstance>();
		if (Instance == nullptr)
		{
			return ;
		}
		UE_LOG(LogTemp, Log, TEXT("실행이 된걸까?"));
		Instance->LoadGameMap();
		
		}),10, false);
}
