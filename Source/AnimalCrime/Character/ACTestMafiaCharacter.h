// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ACCharacter.h"
#include "ACTestMafiaCharacter.generated.h"

/**
 * 
 */
UCLASS()
class ANIMALCRIME_API AACTestMafiaCharacter : public AACCharacter
{
	GENERATED_BODY()

protected:
	void Interact(const FInputActionValue& Value) override;

	void ItemDrop(const FInputActionValue& Value) override;

	UFUNCTION(Server, Reliable)
	void ServerInteract();

	void ServerItemDrop() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bomb")
	uint8 bIsInteractBomb : 1;

	//!<아이템
	UPROPERTY()
	TObjectPtr<class AACEscapeMissionBomb> InteractBomb;
	UPROPERTY()
	TObjectPtr<class AACEscapeMissionBomb> HandBomb;
};
