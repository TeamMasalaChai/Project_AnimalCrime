// Fill out your copyright notice in the Description page of Project Settings.

#include "ACInteractionSubsystem.h"
#include "Interaction/ACInteractionDatabase.h"
#include "AnimalCrime.h"

void UACInteractionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// 1. 경로가 유효한지 확인
	if (DatabasePath.IsValid())
	{
		// 2. 동기 로드 (LoadObject 대신 TryLoad 사용이 더 안전하고 편리함)
		InteractionDatabase = Cast<UACInteractionDatabase>(DatabasePath.TryLoad());

		if (InteractionDatabase)
		{
			UE_LOG(LogTemp, Log, TEXT("Database Loaded to Raw Pointer: %s"), *InteractionDatabase->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to load database from path: %s"), *DatabasePath.ToString());
		}
	}


	// 초기화 로그
	UE_LOG(LogSW, Log, TEXT("[InteractionSubsystem] Initialized"));

	//// 데이터베이스 유효성 검사
	//if (InteractionDatabase == nullptr)
	//{
	//	UE_LOG(LogSW, Warning, TEXT("[InteractionSubsystem] InteractionDatabase is not set! Please assign DA_InteractionDatabase in Project Settings."));
	//}
	//else
	//{
	//	UE_LOG(LogSW, Log, TEXT("[InteractionSubsystem] Database loaded: %s"), *InteractionDatabase->GetName());
	//}
}

void UACInteractionSubsystem::Deinitialize()
{
	UE_LOG(LogSW, Log, TEXT("[InteractionSubsystem] Deinitialized"));

	Super::Deinitialize();
}
