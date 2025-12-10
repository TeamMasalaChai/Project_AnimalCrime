// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "ACMainGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ANIMALCRIME_API AACMainGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	AACMainGameMode();
	
public:
	UFUNCTION(BlueprintCallable)
	void AddTeamScore(int32 Score);
	
	UFUNCTION(BlueprintCallable)
	int32 GetTeamScore() const;
};
