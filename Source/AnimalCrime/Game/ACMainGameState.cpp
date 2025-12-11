// Fill out your copyright notice in the Description page of Project Settings.


#include "ACMainGameState.h"

#include "Net/UnrealNetwork.h"

#include "Character/ACTestMafiaCharacter.h"

void AACMainGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AACMainGameState, TeamScore);
	DOREPLIFETIME(AACMainGameState, EscapeState);
}

void AACMainGameState::ServerChangeEscapeState_Implementation(EEscapeState NewEscapeState)
{
	EscapeState = NewEscapeState;

	switch (EscapeState)
	{
	case EEscapeState::GameStart:
		break;
	case EEscapeState::DeliverBomb:
		break;
	case EEscapeState::Escape:

		//마피아들의 폭탄설치가능구역 Visible 끄기
		for (AACTestMafiaCharacter* Mafia : MafiaPlayers)
		{
			if (Mafia == nullptr)
			{
				continue;
			}
			Mafia->ClientSetBombAreaVisible(false);
			Mafia->ClientSetEscapeAreaVisible(true);
		}
		break;
	default:
		break;
	}
}
