// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/ACGameEnums.h"
#include "Blueprint/UserWidget.h"
#include "ACGameResultScreen.generated.h"

/**
 * 
 */
UCLASS()
class ANIMALCRIME_API UACGameResultScreen : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
public:
	UFUNCTION()
	void SetGameResult(EGameEndType GameEndType);
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> GameResultText;
};
