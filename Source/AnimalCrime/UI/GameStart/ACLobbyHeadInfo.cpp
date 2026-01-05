
#include "UI/GameStart/ACLobbyHeadInfo.h"
#include "Components/TextBlock.h"

void UACLobbyHeadInfo::SetSteamNameText(const FString& InSteamName)
{
	if (SteamNameText == nullptr)
	{
		return;
	}
	SteamNameText->SetText(FText::FromString(InSteamName));
}

void UACLobbyHeadInfo::SetReadyIconVisible(bool bIsVisible)
{

}
