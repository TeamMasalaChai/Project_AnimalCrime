
#include "ACLobbyPlayerController.h"

void AACLobbyPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // 마우스 커서 숨김
    bShowMouseCursor = false;

    // 클릭 끔
    bEnableClickEvents = false;
    bEnableMouseOverEvents = false;

    // 게임 입력만 받기
    SetInputMode(FInputModeGameOnly());
}