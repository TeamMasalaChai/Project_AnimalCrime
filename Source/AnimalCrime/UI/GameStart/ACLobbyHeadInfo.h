
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ACLobbyHeadInfo.generated.h"

UCLASS()
class ANIMALCRIME_API UACLobbyHeadInfo : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetSteamNameText(const FString& InSteamName);
	UFUNCTION(BlueprintCallable)
	void SetReadyIconVisible(bool bIsVisible);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> SteamNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> ReadyIcon;
};
