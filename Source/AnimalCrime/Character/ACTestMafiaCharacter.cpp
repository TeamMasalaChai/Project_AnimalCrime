// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ACTestMafiaCharacter.h"

void AACTestMafiaCharacter::Interact(const FInputActionValue& Value)
{
	if (bIsInteractBomb == true)
	{
		UE_LOG(LogTemp, Log, TEXT("Interact Bomb Success"));
		bIsInteractBomb = false;
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Not Interact Bomb"));
	}
}
