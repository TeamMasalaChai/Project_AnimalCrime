// Fill out your copyright notice in the Description page of Project Settings.


#include "Prison/ACPrisonBase.h"
#include "Character/ACCharacter.h"
#include "Component/ACInteractableComponent.h"
#include "Net/UnrealNetwork.h"
#include "AnimalCrime.h"

AACPrisonBase::AACPrisonBase()
{
	bReplicates = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	InteractBoxComponent = CreateDefaultSubobject<UACInteractableComponent>(TEXT("InteractBoxComponent"));
	InteractBoxComponent->SetupAttachment(RootComponent);
	InteractBoxComponent->SetOffset(FVector(160.f, 0.f, 30.f));
}

void AACPrisonBase::BeginPlay()
{
	Super::BeginPlay();
}

void AACPrisonBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AACPrisonBase, bDoorOpen);
}

void AACPrisonBase::OnRep_DoorOpen()
{
	// 클라이언트에서 문 상태 변경 이벤트 호출
	BP_OnDoorStateChanged(bDoorOpen);
}

bool AACPrisonBase::CanInteract(AACCharacter* ACPlayer)
{
	if (ACPlayer == nullptr)
	{
		return false;
	}

	// 문이 이미 열려있으면 상호작용 불가
	if (bDoorOpen == true)
	{
		return false;
	}

	if (ACPlayer->GetCharacterType() == EACCharacterType::Mafia)
	{
		return true;
	}
	else if (ACPlayer->GetCharacterType() == EACCharacterType::Police)
	{
		return false;
	}

	return false;
}

void AACPrisonBase::OnInteract(AACCharacter* ACPlayer)
{
	ShowInteractDebug(ACPlayer, GetName());

	// 서버에서만 문 열기
	if (HasAuthority())
	{
		OpenDoor();
	}
}

void AACPrisonBase::OpenDoor()
{
	if (bDoorOpen == true)
	{
		return;
	}

	AC_LOG(LogSW, Log, TEXT("Prison Door Opened"));

	bDoorOpen = true;

	// 서버에서도 이벤트 호출 (OnRep은 클라이언트에서만 호출됨)
	BP_OnDoorStateChanged(true);

	// 자동 닫힘 타이머 설정
	GetWorldTimerManager().SetTimer(
		DoorTimerHandle,
		this,
		&AACPrisonBase::CloseDoor,
		AutoCloseDelay,
		false
	);
}

void AACPrisonBase::CloseDoor()
{
	if (bDoorOpen == false)
	{
		return;
	}

	AC_LOG(LogSW, Log, TEXT("Prison Door Closed"));

	bDoorOpen = false;

	// 서버에서도 이벤트 호출
	BP_OnDoorStateChanged(false);

	// 타이머 클리어
	GetWorldTimerManager().ClearTimer(DoorTimerHandle);
}

// todo: 추후에 경찰은 문의 피통 한칸당 5초로 수리?
float AACPrisonBase::GetRequiredHoldTime() const
{
	return 10.0f;
}
