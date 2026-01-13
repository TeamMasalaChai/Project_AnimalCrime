
#include "ACPlayerState.h"
#include "ACMainGameState.h"
#include "ACMainPlayerController.h"
#include "UI/ACHUDWidget.h"
#include "Net/UnrealNetwork.h"
#include "AnimalCrime.h"

void AACPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AACPlayerState, PlayerRole);
	DOREPLIFETIME(AACPlayerState, CharacterLocation);
	DOREPLIFETIME(AACPlayerState, EscapeState);
}

void AACPlayerState::OnRep_EscapeState()
{
	AACMainPlayerController* PC = GetOwner<AACMainPlayerController>();
	if (PC == nullptr || PC->ACHUDWidget == nullptr)
	{
		return;
	}

	PC->ACHUDWidget->UpdateQuestTracker(EscapeState);
}

void AACPlayerState::EnterSpectatorState()
{
	if (IsSpectator() == true)
	{
		return;
	}

	//관전 상태로 전환
	SetIsSpectator(true); 

	CharacterLocation = ECharacterLocation::Escape;

	if (HasAuthority() == false)
	{
		return;
	}

	AACMainGameState* GS = GetWorld()->GetGameState<AACMainGameState>();
	if (GS == nullptr)
	{
		return;
	}

	AController* PC = GetOwner<AController>();
	if (PC == nullptr)
	{
		return;
	}

	//자신을 관전 대상에서 제외
	GS->RemoveSpectatablePawn(PC->GetPawn());
}