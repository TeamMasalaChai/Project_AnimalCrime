
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ACEscapeMissionBomb.generated.h"

UCLASS()
class ANIMALCRIME_API AACEscapeMissionBomb : public AActor
{
	GENERATED_BODY()
	
public:	
	AACEscapeMissionBomb();

protected:
	virtual void BeginPlay() override;

protected:

	UFUNCTION()
	void OnInteractTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnInteractTriggerOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	TObjectPtr<class UStaticMeshComponent> BombMeshComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact")
	TObjectPtr<class UBoxComponent> InteractBoxComponent;
};
