// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_CheckInitialSkill.generated.h"

/**
 * 
 */
UCLASS()
class ANIMALCRIME_API UBTDecorator_CheckInitialSkill : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UBTDecorator_CheckInitialSkill();
	
public:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
