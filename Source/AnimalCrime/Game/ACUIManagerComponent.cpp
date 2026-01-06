
#include "Game/ACUIManagerComponent.h"
#include "UI/Common/ACNotificationText.h"
UACUIManagerComponent::UACUIManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	static ConstructorHelpers::FClassFinder<UACNotificationText> NotificationTextRef(
		TEXT("/Game/Project/UI/Common/WBP_NotificationText.WBP_NotificationText_C"));
	if (NotificationTextRef.Succeeded())
	{
		NotificationWidgetClass = NotificationTextRef.Class;
		UE_LOG(LogTemp, Warning, TEXT("NotificationWidgetClass loaded successfully"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load NotificationWidgetClass"));
	}
}

void UACUIManagerComponent::ShowNotification(const FText& Text)
{
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (PC == nullptr || PC->IsLocalController() == false)
	{
		return;
	}

	if (NotificationWidgetClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("NotificationWidgetClass is nullptr"));
		return;
	}

	UACNotificationText* Widget = CreateWidget<UACNotificationText>(PC, NotificationWidgetClass);
	if (Widget == nullptr)
	{
		return;
	}

	Widget->SetNotificationText(Text);
	Widget->AddToViewport();
}