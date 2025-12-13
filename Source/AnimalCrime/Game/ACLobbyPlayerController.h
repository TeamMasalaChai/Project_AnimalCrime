
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ACLobbyPlayerController.generated.h"

UCLASS()
class ANIMALCRIME_API AACLobbyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
};
