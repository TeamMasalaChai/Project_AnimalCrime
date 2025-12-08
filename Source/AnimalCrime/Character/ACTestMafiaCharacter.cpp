
#include "Character/ACTestMafiaCharacter.h"
#include "Item/ACEscapeMissionBomb.h"
#include "Components/StaticMeshComponent.h"

void AACTestMafiaCharacter::Interact(const FInputActionValue& Value)
{
	ServerInteract(); //서버에 알림.
}

void AACTestMafiaCharacter::ItemDrop(const FInputActionValue& Value)
{
    UE_LOG(LogTemp, Log, TEXT("AACTestMafiaCharacter::ItemDrop!!"));
	ServerItemDrop(); //서버에 알림.
}

void AACTestMafiaCharacter::ServerInteract_Implementation()
{
    if (InteractBomb == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Not Interact Bomb"));
        return;
    }

    USkeletalMeshComponent* MeshComp = GetMesh();
    if (MeshComp == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("Interact: Mesh is NULL"));
        return;
    }

    const FName SocketName = TEXT("CarryingSpine");

    if (MeshComp->DoesSocketExist(SocketName) == false)
    {
        UE_LOG(LogTemp, Error, TEXT("Interact: Socket '%s' DOES NOT EXIST"), *SocketName.ToString());
        return;
    }

    // 폭탄 물리 끄기
    UStaticMeshComponent* BombMesh = InteractBomb->GetBombMeshComp();
	if (BombMesh == nullptr)
    {
		UE_LOG(LogTemp, Error, TEXT("Interact: BombMesh is NULL"));
        return;
    }
    BombMesh->SetSimulatePhysics(false);

	//서버: 폭탄을 캐릭터에 부착
    InteractBomb->AttachToComponent(
        MeshComp,
        FAttachmentTransformRules::SnapToTargetNotIncludingScale,
        SocketName
    );

    InteractBomb->AttachedCharacter = this;
    
    //아이템 핸드로 이동
    HandBomb = InteractBomb;

    InteractBomb->SetActorEnableCollision(false);
    UE_LOG(LogTemp, Log, TEXT("Interact Bomb Success"));
}

void AACTestMafiaCharacter::ServerItemDrop()
{
    if (HandBomb != nullptr)
    {
        // 1. Detach
        HandBomb->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

        // 2. 충돌 다시 활성화
        HandBomb->SetActorEnableCollision(true);

        // 폭탄 물리 켜기
        UStaticMeshComponent* BombMesh = InteractBomb->GetBombMeshComp();
        if (BombMesh == nullptr)
        {
            UE_LOG(LogTemp, Error, TEXT("Interact: BombMesh is NULL"));
            return;
        }
        BombMesh->SetSimulatePhysics(true);

        // 3. 앞으로 던지기
        FVector ThrowDirection = GetActorForwardVector() + FVector(0, 0, 0.3f);
        BombMesh->AddImpulse(ThrowDirection * 300.0f, NAME_None, true);

        // 4. Replicated 변수 초기화 (클라이언트에서도 Detach 되도록)
        HandBomb->AttachedCharacter = nullptr;

        // 5. 참조 해제
        HandBomb = nullptr;
        bIsInteractBomb = false;

        UE_LOG(LogTemp, Log, TEXT("ServerDropBomb: Bomb dropped"));
    }
    else
    {
		UE_LOG(LogTemp, Warning, TEXT("ServerDropBomb: No bomb to drop"));
    }
}