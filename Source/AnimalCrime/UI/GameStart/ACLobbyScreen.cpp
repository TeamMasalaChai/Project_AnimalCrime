
#include "UI/GameStart/ACLobbyScreen.h"
#include "Components/TextBlock.h"
#include "Game/ACLobbyGameState.h"
#include "Game/ACLobbyPlayerController.h"
#include "Components/SizeBox.h"
#include "AnimalCrime.h"

void UACLobbyScreen::NativeConstruct()
{
	Super::NativeConstruct();
	AACLobbyGameState* GS = GetWorld()->GetGameState<AACLobbyGameState>();
	if (GS == nullptr)
	{	
		return;
	}
	
	//ReadyPlayer, Player 수가 변경되면 호출되도록 게임 스테이트 구독
	GS->OnPlayerCountChanged.AddDynamic(this, &UACLobbyScreen::UpdatePlayerReadyText);

	// 처음에는 ReadyPlayer, Player 수 직접 갱신
	UpdatePlayerReadyText(GS->GetReadyPlayerCount(), GS->GetAllPlayerCount());
}

void UACLobbyScreen::UpdatePlayerReadyText(int32 ReadyPlayerNum, int32 AllPlayerNum)
{
	if (ReadyPlayerText == nullptr)
	{
		UE_LOG(LogSY, Warning, TEXT("ReadyPlayerText is nullptr"));
		return;
	}
	UE_LOG(LogSY, Log, TEXT("Update UI Screen"));
	ReadyPlayerText->SetText( FText::Format(FText::FromString(TEXT("{0}/{1}")), ReadyPlayerNum, AllPlayerNum));

	AACLobbyPlayerController* PC = Cast<AACLobbyPlayerController>(GetOwningPlayer());
	if (PC == nullptr)
	{
		return;
	}
	if (PC->HasAuthority() && PC->IsLocalController())
	{
		UE_LOG(LogSY, Warning, TEXT("host"));
		// 모든 인원이 준비 되었을 때 호스트만 시작 텍스트 보이기
		SetStartTextVisibility(ReadyPlayerNum == AllPlayerNum);
	}
	else
	{
		UE_LOG(LogSY, Warning, TEXT("client"));
		//자신이 준비를 안했고 호스	트가 아닌 경우에만 준비 텍스트 보이기
		SetReadyTextVisibility(!PC->GetIsReady());
	}
}

void UACLobbyScreen::SetStartTextVisibility(bool bShowText)
{
	if (StartTextSizeBox == nullptr)
	{
		return;
	}

	StartTextSizeBox->SetVisibility(bShowText ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UACLobbyScreen::SetReadyTextVisibility(bool bShowText)
{
	if (ReadyTextSizeBox == nullptr)
	{
		return;
	}

	ReadyTextSizeBox->SetVisibility(bShowText ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}
