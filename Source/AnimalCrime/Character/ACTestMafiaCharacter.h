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

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bomb")
	uint8 bIsInteractBomb : 1;

protected:
	void Interact(const FInputActionValue& Value) override;
};
