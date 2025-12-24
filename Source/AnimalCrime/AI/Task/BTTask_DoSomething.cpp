// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_DoSomething.h"

#include "AnimalCrime.h"
#include "AI/ACCitizenAIController.h"
#include "Character/ACCitizen.h"

EBTNodeResult::Type UBTTask_DoSomething::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type SuperResult = Super::ExecuteTask(OwnerComp, NodeMemory); 

	AACCitizenAIController* CitizenAIController = Cast<AACCitizenAIController>(OwnerComp.GetAIOwner());
	if (CitizenAIController == nullptr)
	{
		UE_LOG(LogHY, Error, TEXT("CitizenAIController is nullptr"));
		return EBTNodeResult::Failed; 
	}
	
	AACCitizen* CitizenPawn = Cast<AACCitizen>(CitizenAIController->GetPawn());
	if (CitizenPawn == nullptr)
	{
		UE_LOG(LogHY, Error, TEXT("CitizenPawn is nullptr"));
		return EBTNodeResult::Failed;
	}
	
	if (CitizenPawn->DetectPolice())
	{
		CitizenPawn->RunFromPolice();
		return EBTNodeResult::Succeeded;	
	}
	return EBTNodeResult::Succeeded;
}
