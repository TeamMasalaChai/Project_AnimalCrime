
#include "ACMainPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "ACTitlePlayerController.h"
#include "UI/Shop/ACShopWidget.h"
#include "AnimalCrime.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/ACCharacter.h"

AACMainPlayerController::AACMainPlayerController()
{
	//탈출 스크린 로드
	static ConstructorHelpers::FClassFinder<UUserWidget> EscapeScreenRef(TEXT("/Game/Project/UI/WBP_EscapeText.WBP_EscapeText_C"));
	if (EscapeScreenRef.Succeeded())
	{
		EscapeScreenClass = EscapeScreenRef.Class;
	}
}

void AACMainPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//ConsoleCommand(TEXT("show Collision"));
	ConsoleCommand(TEXT("Stat FPS"));
}

void AACMainPlayerController::ShowEscapeUI_Implementation()
{
    EscapeScreen = CreateWidget<UUserWidget>(this, EscapeScreenClass);
    if (EscapeScreen == nullptr)
    {
        return;
    }
    EscapeScreen->AddToViewport();
}



void AACMainPlayerController::ClientToggleShopWidget_Implementation(TSubclassOf<class UACShopWidget> WidgetClass)
{
    if (WidgetClass == nullptr)
    {
        UE_LOG(LogHG, Error, TEXT("ClientToggleShopWidget: WidgetClass is null"));
        return;
    }

    // 이미 위젯이 열려있으면 닫기
    if (CurrentShopWidget != nullptr && CurrentShopWidget->IsInViewport())
    {
        CloseShop();
    }
    else
    {
        // ===== 상점 열기 =====
        UE_LOG(LogHG, Log, TEXT("Client: Opening Shop UI"));

        UACShopWidget* ShopWidget = CreateWidget<UACShopWidget>(GetWorld(), WidgetClass);

        if (ShopWidget != nullptr)
        {
            ShopWidget->AddToViewport();
            CurrentShopWidget = ShopWidget;

            // 델리게이트 바인딩
            ShopWidget->OnCloseRequested.AddDynamic(this, &AACMainPlayerController::CloseShop);

            // 상점용 카메라로 전환
            SetShopCamera();

            // 입력모드를 게임,UI로 변경
            FInputModeGameAndUI InputMode;
            InputMode.SetWidgetToFocus(ShopWidget->TakeWidget());
            InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            InputMode.SetHideCursorDuringCapture(false); // 클릭해도 마우스 커서 계속 보이도록
            SetInputMode(InputMode);

            SetShowMouseCursor(true); // 마우스 커서 보이도록
  
            // 상점의 슬롯들 로드
            ShopWidget->LoadAndCreateSlots(TEXT("/Game/Project/Item/"));

            // Character의 입력 모드도 변경
            AACCharacter* ControlledCharacter = GetPawn<AACCharacter>();
            if (ControlledCharacter != nullptr)
            {
                ControlledCharacter->ChangeInputMode(EInputMode::Settings);
            }
        }
    }
}

void AACMainPlayerController::CloseShop()
{
    if (CurrentShopWidget == nullptr || CurrentShopWidget->IsInViewport() == false)
    {
        // 이미 닫혀있음
        return;
    }

    // ===== 상점 닫기 =====
    UE_LOG(LogHG, Log, TEXT("Client: Closing Shop UI"));

    CurrentShopWidget->RemoveFromParent();
    CurrentShopWidget = nullptr;

    // 카메라 복원
    RestoreOriginalCamera();

    // 마우스 숨기기
    SetShowMouseCursor(false);

    // ===== 입력 모드를 게임 모드로 전환 =====
    FInputModeGameOnly InputMode;
    InputMode.SetConsumeCaptureMouseDown(false);
    SetInputMode(InputMode);

    // Character의 입력 모드도 변경
    AACCharacter* ControlledCharacter = GetPawn<AACCharacter>();
    if (ControlledCharacter != nullptr)
    {
        ControlledCharacter->ChangeInputMode(EInputMode::Sholder);
    }
}

void AACMainPlayerController::SetShopCamera()
{
    if (bShopCameraActive == true)
    {
        // 이미 상점 카메라 모드
        return;
    }

    AACCharacter* ControlledCharacter = GetPawn<AACCharacter>();
    if (ControlledCharacter == nullptr)
    {
        return;
    }

    // CameraBoom 가져오기
    USpringArmComponent* CameraBoom = ControlledCharacter->FindComponentByClass<USpringArmComponent>();
    if (CameraBoom == nullptr)
    {
        UE_LOG(LogHG, Error, TEXT("CameraBoom not found on Character"));
        return;
    }

    // ===== 현재 카메라 상태 저장 =====
    SavedControlRotation = GetControlRotation();
    SavedTargetArmLength = CameraBoom->TargetArmLength;
    SavedTargetOffset = CameraBoom->TargetOffset;
    SavedRelativeLocation = CameraBoom->GetRelativeLocation();
    SavedRelativeRotation = CameraBoom->GetRelativeRotation();
    bSavedUsePawnControlRotation = CameraBoom->bUsePawnControlRotation;

    // ===== 월드 기준 고정 각도로 카메라 설정 =====

    // CameraBoom이 컨트롤러 회전을 따라가지 않도록 설정
    CameraBoom->bUsePawnControlRotation = false;

    // 캐릭터 정면(월드 기준 북쪽)에서 약간 위에서 보도록 설정
    FRotator WorldFixedRotation = FRotator(-15.f, 0.f, 0.f);
    SetControlRotation(WorldFixedRotation);

    // CameraBoom의 로컬 회전을 설정 (캐릭터를 정면에서 봄)
    CameraBoom->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));
    CameraBoom->SetRelativeLocation(FVector(100.0f, 100.0f, 0.f));

    // 카메라 거리 및 오프셋 조정
    CameraBoom->TargetArmLength = 300.f;
    CameraBoom->TargetOffset = FVector(0.f, 0.f, 50.f);

    bShopCameraActive = true;
}

void AACMainPlayerController::RestoreOriginalCamera()
{
    if (bShopCameraActive == false)
    {
        return;  // 상점 카메라 모드가 아님
    }

    AACCharacter* ControlledCharacter = GetPawn<AACCharacter>();
    if (ControlledCharacter == nullptr)
    {
        return;
    }

    // CameraBoom 가져오기
    USpringArmComponent* CameraBoom = ControlledCharacter->FindComponentByClass<USpringArmComponent>();
    if (CameraBoom == nullptr)
    {
        return;
    }

    // ===== 원래 카메라 상태 복원 =====

    // 컨트롤러 회전 복원
    SetControlRotation(SavedControlRotation);

    // CameraBoom 설정 복원
    CameraBoom->bUsePawnControlRotation = bSavedUsePawnControlRotation;
    CameraBoom->SetRelativeRotation(SavedRelativeRotation);
    CameraBoom->SetRelativeLocation(SavedRelativeLocation);
    CameraBoom->TargetArmLength = SavedTargetArmLength;
    CameraBoom->TargetOffset = SavedTargetOffset;

    bShopCameraActive = false;
}