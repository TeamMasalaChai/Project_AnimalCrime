
#include "ACEscapeMissionBomb.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Character/ACTestMafiaCharacter.h"
#include "Net/UnrealNetwork.h"

AACEscapeMissionBomb::AACEscapeMissionBomb()
{
	BombMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bomb"));
	BombMeshComp->SetRelativeScale3D(FVector(0.4f, 0.4f, 0.4f));
	RootComponent = BombMeshComp;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BombMeshRef(TEXT("/Game/wooden_barrier_setlow_poly/StaticMeshes/Object_12.Object_12"));
	if (BombMeshRef.Succeeded())
	{
		BombMeshComp->SetStaticMesh(BombMeshRef.Object);
	}

	//물리 및 중력 설정
	BombMeshComp->SetSimulatePhysics(true);	// 물리 시뮬레이션 활성화
	BombMeshComp->SetEnableGravity(true);	// 중력 활성화
	BombMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BombMeshComp->SetCollisionProfileName(TEXT("PhysicsActor"));

	InteractBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractBoxComponent"));
	InteractBoxComponent->SetupAttachment(RootComponent);

	// 네트워크 복제 설정. 서버에 이 액터가 생성되면 클라이언트로 복제됨, 삭제도 동기화
	bReplicates = true;

	// 이동 복제 활성화
	SetReplicateMovement(true);
}

void AACEscapeMissionBomb::BeginPlay()
{
	Super::BeginPlay();
	InteractBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AACEscapeMissionBomb::OnInteractTriggerOverlapBegin);
	InteractBoxComponent->OnComponentEndOverlap.AddDynamic(this, &AACEscapeMissionBomb::OnInteractTriggerOverlapEnd);

	// 루트 컴포넌트의 로컬 바운드 가져오기
	FBoxSphereBounds RootBounds = BombMeshComp->Bounds;

	// BoxExtent 설정 (약간 여유 포함)
	FVector Margin(100.f, 100.f, 100.f); // 필요에 따라 조정
	InteractBoxComponent->SetBoxExtent(RootBounds.BoxExtent + Margin);

	// 박스 위치 루트에 맞추기
	InteractBoxComponent->SetRelativeLocation(FVector::ZeroVector);
}

void AACEscapeMissionBomb::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AACEscapeMissionBomb, AttachedCharacter);
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
	TestMafiaChar->InteractBomb = this;
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
	TestMafiaChar->InteractBomb = nullptr;
}

void AACEscapeMissionBomb::OnRep_AttachedCharacter()
{
	if(AttachedCharacter == nullptr)
	{
		return;
	}

	USkeletalMeshComponent* MeshComp = AttachedCharacter->GetMesh();
	if (MeshComp == nullptr || MeshComp->DoesSocketExist(TEXT("CarryingSpine")) == false)
	{
		return;
	}

	// 폭탄 물리 끄기
	if (BombMeshComp == nullptr)
	{
		return;
	}
	BombMeshComp->SetSimulatePhysics(false);

	AttachToComponent(
		MeshComp,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		TEXT("CarryingSpine")
	);
	SetActorEnableCollision(false);

}

