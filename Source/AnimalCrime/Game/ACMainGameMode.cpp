

#include "ACMainGameMode.h"

#include "ACMainGameState.h"
#include "ACMainPlayerController.h"
#include "Character/ACCharacter.h"
#include "Character/ACTestMafiaCharacter.h"
AACMainGameMode::AACMainGameMode()
{
	PlayerControllerClass = AACMainPlayerController::StaticClass();
	//DefaultPawnClass = AACCharacter::StaticClass();
	DefaultPawnClass = AACTestMafiaCharacter::StaticClass();
	GameStateClass = AACMainGameState::StaticClass();
}

void AACMainGameMode::AddTeamScore(int32 Score)
{
	UE_LOG(LogTemp, Log, TEXT("[AddTeamScore]"));
	AACMainGameState* MainGameState = GetGameState<AACMainGameState>();
	if (MainGameState)
	{
		MainGameState->TeamScore += Score;  // Replicate → 모든 클라 업데이트됨
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT(" AddTeamScore Nullptr"));
	}
}

int32 AACMainGameMode::GetTeamScore() const
{
	UE_LOG(LogTemp, Log, TEXT("[GetTeamScore]"));
	AACMainGameState* MainGameState = GetGameState<AACMainGameState>();
	if (MainGameState)
	{
		return MainGameState->TeamScore;
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT(" GetTeamScore Nullptr"));
	}
	return -1;
}
