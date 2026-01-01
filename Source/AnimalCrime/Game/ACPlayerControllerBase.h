#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ACPlayerControllerBase.generated.h"

UCLASS()
class ANIMALCRIME_API AACPlayerControllerBase : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(Client, Reliable)
	void Client_CleanupVoiceBeforeTravel();

	UFUNCTION(Server, Reliable)
	void Server_NotifyVoiceCleaned();

private:
	void CheckClientVoiceCleanupComplete();

private:
	int32 ClientPollingAttempts = 0;
	static constexpr int32 MaxClientPollingAttempts = 50; // 최대 500ms (50 * 10ms)
	static constexpr float ClientPollingInterval = 0.01f; // 10ms
};
