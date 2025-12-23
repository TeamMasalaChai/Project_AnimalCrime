// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/ACInteractInterface.h"
#include "ACPrisonBase.generated.h"

UCLASS()
class ANIMALCRIME_API AACPrisonBase : public AActor, public IACInteractInterface
{
	GENERATED_BODY()

public:
	AACPrisonBase();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_DoorOpen();

protected: //!< 상호작용 인터페이스 구현
	virtual bool CanInteract(class AACCharacter* ACPlayer) override;
	virtual void OnInteract(class AACCharacter* ACPlayer) override;
	virtual float GetRequiredHoldTime() const override;

public:
	/**
		@brief 문 열기 (서버에서 호출)
	**/
	UFUNCTION(BlueprintCallable, Category = "Door")
	void OpenDoor();

	/**
		@brief 문 닫기 (서버에서 호출)
	**/
	UFUNCTION(BlueprintCallable, Category = "Door")
	void CloseDoor();

protected:
	/**
		@brief 문 상태 변경 시 블루프린트에서 구현할 이벤트
		@param bOpen - true면 열림, false면 닫힘
	**/
	UFUNCTION(BlueprintImplementableEvent, Category = "Door")
	void BP_OnDoorStateChanged(bool bOpen);

	//!< 상호작용 멤버변수
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact")
	TObjectPtr<class UACInteractableComponent> InteractBoxComponent;

	//!< 문 상태
protected:
	UPROPERTY(ReplicatedUsing = OnRep_DoorOpen, BlueprintReadOnly, Category = "Door")
	bool bDoorOpen = false;

	/** 문이 자동으로 닫히는 시간 (초) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	float AutoCloseDelay = 5.0f;

private:
	FTimerHandle DoorTimerHandle;
};
