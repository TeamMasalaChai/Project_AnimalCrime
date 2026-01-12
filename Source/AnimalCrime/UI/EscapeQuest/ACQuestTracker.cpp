
#include "UI/EscapeQuest/ACQuestTracker.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "AnimalCrime.h"

void UACQuestTracker::NativeConstruct()
{
	Super::NativeConstruct();
}

void UACQuestTracker::UpdateQuest(EEscapeState NewState)
{

	if (!ensure(QuestText1 && QuestText2 && QuestText3 && QuestText4 &&
		Strikethrough1 && Strikethrough2 && Strikethrough3 && Strikethrough4))
	{
		UE_LOG(LogSY, Log, TEXT("UpdateQuest ensure"));
		return;
	}

	UE_LOG(LogSY, Log, TEXT("Update Quest"));

	switch (NewState)
	{
	case EEscapeState::Contraband:
		break;
	case EEscapeState::BlackMarket:
		QuestText1->SetColorAndOpacity(FLinearColor(0.f, 1.f, 0.f, 1.f));
		Strikethrough1->SetVisibility(ESlateVisibility::Visible);
		QuestText2->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
		break;
	case EEscapeState::DeliverBomb:
		QuestText2->SetColorAndOpacity(FLinearColor(0.f, 1.f, 0.f, 1.f));
		Strikethrough2->SetVisibility(ESlateVisibility::Visible);
		QuestText3->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
		break;
	case EEscapeState::Escape:
		QuestText1->SetColorAndOpacity(FLinearColor(0.f, 1.f, 0.f, 1.f));
		Strikethrough1->SetVisibility(ESlateVisibility::Visible);
		QuestText2->SetColorAndOpacity(FLinearColor(0.f, 1.f, 0.f, 1.f));
		Strikethrough2->SetVisibility(ESlateVisibility::Visible);
		QuestText3->SetColorAndOpacity(FLinearColor(0.f, 1.f, 0.f, 1.f));
		Strikethrough3->SetVisibility(ESlateVisibility::Visible);
		QuestText4->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
		break;
	}
}