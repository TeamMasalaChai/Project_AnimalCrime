
#include "UI/GameResult/ACGameResultScreen.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "ACGameResultWinnerBox.h"
#include "Game/ACMainGameState.h"
#include "AnimalCrime.h"

void UACGameResultScreen::NativeConstruct()
{
	Super::NativeConstruct();

	if (FadeInAnim == nullptr)
	{
		return;
	}

	PlayAnimation(FadeInAnim);
}

void UACGameResultScreen::SetGameResult(EGameEndType GameEndType)
{
	if (GameResultText == nullptr)
	{
		UE_LOG(LogSY, Log, TEXT("GameResultText is nullptr"));
		return;
	}

	AACMainGameState* GS = GetWorld()->GetGameState<AACMainGameState>();
	if (GS == nullptr)
	{
		UE_LOG(LogSY, Log, TEXT("GameState is nullptr"));
		return;
	}

	TArray< class AACPlayerState*> Winners;

	if (GameEndType == EGameEndType::AllPrison || GameEndType == EGameEndType::TimeOver)
	{
		GameResultText->SetText(FText::FromString(TEXT("Police Win")));
		if(WinnerBoxClass==nullptr || WinnerBoxes==nullptr)
		{
			UE_LOG(LogSY, Log, TEXT("WinnerBoxClass or WinnerBoxes is nullptr"));
			return;
		}
		Winners = GS->GetPlayersByRoleAndLocation(EPlayerRole::Police, ECharacterLocation::None);
		if (PoliceWinTexture == nullptr || RoleBackGround == nullptr)
		{
			UE_LOG(LogSY, Log, TEXT("MafiaWinTexture or RoleBackGround is nullptr"));
			return;
		}
		RoleBackGround->SetBrushFromTexture(PoliceWinTexture);
	}
	else if (GameEndType == EGameEndType::Escape)
	{
		GameResultText->SetText(FText::FromString(TEXT("Mafia Win")));

		Winners = GS->GetPlayersByRoleAndLocation(EPlayerRole::Mafia, ECharacterLocation::Escape);

		if(MafiaWinTexture == nullptr || RoleBackGround == nullptr)
		{
			UE_LOG(LogSY, Log, TEXT("MafiaWinTexture or RoleBackGround is nullptr"));
			return;
		}
		RoleBackGround->SetBrushFromTexture(MafiaWinTexture);
	}
	else if(GameEndType == EGameEndType::Score)
	{
		GameResultText->SetText(FText::FromString(TEXT("Mafia Win")));
		Winners = GS->GetPlayersByRoleAndLocation(EPlayerRole::Mafia, ECharacterLocation::None);

		if (MafiaWinTexture == nullptr || RoleBackGround == nullptr)
		{
			UE_LOG(LogSY, Log, TEXT("MafiaWinTexture or RoleBackGround is nullptr"));
			return;
		}
		RoleBackGround->SetBrushFromTexture(MafiaWinTexture);
	}

	for (AACPlayerState* PS : Winners)
	{
		UACGameResultWinnerBox* WinnerBox = CreateWidget<UACGameResultWinnerBox>(GetWorld(), WinnerBoxClass);
		if (WinnerBox == nullptr)
		{
			UE_LOG(LogSY, Log, TEXT("WinnerBox is nullptr"));
			continue;
		}
		WinnerBox->SetWinnerInfo(PS);
		WinnerBoxes->AddChild(WinnerBox);
	}
}
