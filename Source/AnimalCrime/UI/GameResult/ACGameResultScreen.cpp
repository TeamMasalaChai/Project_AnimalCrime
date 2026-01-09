
#include "UI/GameResult/ACGameResultScreen.h"
#include "Components/TextBlock.h"
#include "AnimalCrime.h"

void UACGameResultScreen::NativeConstruct()
{
	Super::NativeConstruct();

}

void UACGameResultScreen::SetGameResult(EGameEndType GameEndType)
{
	if (GameResultText == nullptr)
	{
		UE_LOG(LogSY, Log, TEXT("GameResultText is nullptr"));
		return;
	}
	if (GameEndType == EGameEndType::AllPrison || GameEndType == EGameEndType::TimeOver)
	{
		GameResultText->SetText(FText::FromString(TEXT("경찰 승리")));
	}
	else
	{
		GameResultText->SetText(FText::FromString(TEXT("마피아 승리")));
	}
}
