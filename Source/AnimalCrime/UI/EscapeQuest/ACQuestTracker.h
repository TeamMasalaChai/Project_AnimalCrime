
#pragma once

#include "CoreMinimal.h"
#include "Game/ACGameEnums.h"
#include "Blueprint/UserWidget.h"
#include "ACQuestTracker.generated.h"

UCLASS()
class ANIMALCRIME_API UACQuestTracker : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	void NativeConstruct() override;
public:
	void UpdateQuest(EEscapeState NewState);
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> QuestText1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> QuestText2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> QuestText3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> QuestText4;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Strikethrough1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Strikethrough2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Strikethrough3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Strikethrough4;
};
