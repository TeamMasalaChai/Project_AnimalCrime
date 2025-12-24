// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorator/BTDecorator_CheckInitialSkill.h"

#include "AnimalCrime.h"
#include "AI/ACCitizenAIController.h"
#include "Character/ACCitizen.h"

UBTDecorator_CheckInitialSkill::UBTDecorator_CheckInitialSkill()
{
}

bool UBTDecorator_CheckInitialSkill::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool SuperResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
	
	AACCitizenAIController* AIController = Cast<AACCitizenAIController>(OwnerComp.GetAIOwner());
	if (AIController == nullptr)
	{
		UE_LOG(LogHY, Error, TEXT("AIController is nullptr"));
		return false;
	}
	
	AACCitizen* CitizenPawn = Cast<AACCitizen>(AIController->GetPawn());
	if (CitizenPawn == nullptr)
	{
		UE_LOG(LogHY, Error, TEXT("CitizenPawn is nullptr"));
		return false;
	}
	
	return !CitizenPawn->IsInitialSkillBlocked(); 
}
