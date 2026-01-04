// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ACInteractionSubsystem.generated.h"

// 전방 선언
class UACInteractionDatabase;

/**
 * @brief 상호작용 시스템을 관리하는 GameInstance Subsystem
 *
 * @details
 * - GameInstance 생명주기 동안 유지됨 (레벨 전환 시에도 유지)
 * - 중앙 상호작용 데이터베이스 관리
 * - 전역적으로 접근 가능 (클라이언트/서버 모두)
 *
 * 사용 방법:
 * UACInteractionSubsystem* InteractionSys =
 *     GetGameInstance()->GetSubsystem<UACInteractionSubsystem>();
 * UACInteractionDatabase* DB = InteractionSys->GetInteractionDatabase();
 */
UCLASS(Blueprintable, Config = Game)
class ANIMALCRIME_API UACInteractionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/**
	 * @brief Subsystem 초기화
	 * @details GameInstance 생성 시 자동 호출
	 */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/**
	 * @brief Subsystem 종료
	 * @details GameInstance 파괴 시 자동 호출
	 */
	virtual void Deinitialize() override;

	/**
	 * @brief 상호작용 데이터베이스 가져오기
	 * @return 중앙 상호작용 데이터베이스 (nullptr일 수 있음)
	 */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	UACInteractionDatabase* GetInteractionDatabase() const { return InteractionDatabase; }

private:
	/** * 1. 설정용 변수 (ini에서 경로만 읽어옴)
	 * FSoftObjectPath는 경로 문자열을 안전하게 담는 구조체입니다.
	 */
	UPROPERTY(Config)
	FSoftObjectPath DatabasePath;

	/** * 2. 런타임용 일반 포인터 (실제 사용)
	 * 중요: UPROPERTY()가 없으면 언리얼이 "사용 안 하네?" 하고 메모리에서 삭제해버립니다 (Crash 원인).
	 */
	UPROPERTY(Transient) // Transient: 이 포인터 자체는 디스크에 저장(직렬화)하지 않음
		UACInteractionDatabase* InteractionDatabase;
};
