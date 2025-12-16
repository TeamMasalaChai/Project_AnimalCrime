#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "ACLobbyGameMode.generated.h"

UCLASS()
class ANIMALCRIME_API AACLobbyGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	AACLobbyGameMode();
	virtual  void BeginPlay() override;

	FTimerHandle TravelTimerHandle;
	void StartGameTravel();
};
