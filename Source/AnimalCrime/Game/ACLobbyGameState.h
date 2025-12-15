
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "ACLobbyGameState.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerCountChanged, int32, ReadyPlayerCount, int32, AllPlayerCount);

UCLASS()
class ANIMALCRIME_API AACLobbyGameState : public AGameState
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
public:
	void UpdateReadyPlayer();

	UFUNCTION()
	void OnRep_PlayerCount();

	UPROPERTY()
	FOnPlayerCountChanged OnPlayerCountChanged;

	FORCEINLINE int32 GetAllPlayerCount() { return AllPlayerCount; }
	FORCEINLINE int32 GetReadyPlayerCount() { return ReadyPlayerCount; }
protected:
	UPROPERTY(ReplicatedUsing = OnRep_PlayerCount)
	int32 AllPlayerCount = 0;
	UPROPERTY(ReplicatedUsing = OnRep_PlayerCount)
	int32 ReadyPlayerCount = 0;
};
