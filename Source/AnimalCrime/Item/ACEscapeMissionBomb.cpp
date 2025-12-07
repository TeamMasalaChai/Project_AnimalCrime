
#include "ACEscapeMissionBomb.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Character/ACTestMafiaCharacter.h"

AACEscapeMissionBomb::AACEscapeMissionBomb()
{
	BombMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bomb"));
	BombMeshComp->SetRelativeScale3D(FVector(0.8f, 0.8f, 0.8f));
	RootComponent = BombMeshComp;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BombMeshRef(TEXT("/Game/wooden_barrier_setlow_poly/StaticMeshes/Object_12.Object_12"));
	if (BombMeshRef.Succeeded())
	{
		BombMeshComp->SetStaticMesh(BombMeshRef.Object);
	}

	InteractBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractBoxComponent"));
	InteractBoxComponent->SetupAttachment(RootComponent);
}

void AACEscapeMissionBomb::BeginPlay()
{
	Super::BeginPlay();

	InteractBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AACEscapeMissionBomb::OnInteractTriggerOverlapBegin);
	InteractBoxComponent->OnComponentEndOverlap.AddDynamic(this, &AACEscapeMissionBomb::OnInteractTriggerOverlapEnd);

	// 루트 컴포넌트의 로컬 바운드 가져오기
	FBoxSphereBounds RootBounds = RootComponent->CalcBounds(RootComponent->GetComponentTransform());

	// BoxExtent 설정 (약간 여유 포함)
	FVector Margin(50.f, 50.f, 50.f); // 필요에 따라 조정
	InteractBoxComponent->SetBoxExtent(RootBounds.BoxExtent + Margin);

	// 박스 위치 루트에 맞추기
	InteractBoxComponent->SetRelativeLocation(FVector::ZeroVector);
}

void AACEscapeMissionBomb::OnInteractTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Log, TEXT("InteractBox Begin"));
	AACTestMafiaCharacter* TestMafiaChar = Cast<AACTestMafiaCharacter>(OtherActor);
	if (TestMafiaChar == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("Not TestMafiaCharacter"));
		return;
	}

	TestMafiaChar->bIsInteractBomb = true;

}

void AACEscapeMissionBomb::OnInteractTriggerOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Log, TEXT("InteractBox End"));

	AACTestMafiaCharacter* TestMafiaChar = Cast<AACTestMafiaCharacter>(OtherActor);
	if (TestMafiaChar == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("Not TestMafiaCharacter"));
		return;
	}

	TestMafiaChar->bIsInteractBomb = false;
}

