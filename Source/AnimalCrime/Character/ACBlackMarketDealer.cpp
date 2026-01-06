
#include "ACBlackMarketDealer.h"
#include "ACMafiaCharacter.h"
#include "Components/BoxComponent.h"
#include "EscapeQuest/ACBlackMarketComponent.h"
#include "Component/ACInteractableComponent.h"
#include "Game/ACMainPlayerController.h"
#include "AnimalCrime.h"

AACBlackMarketDealer::AACBlackMarketDealer()
{
	//HeadMeshComp->SetSkeletalMesh(LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/Creative_Characters_FREE/Skeleton_Meshes/SK_Hairstyle_male_010.SK_Hairstyle_male_010")));
	//TopMesh->SetSkeletalMesh(nullptr);
	//BottomMesh->SetSkeletalMesh(LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/Creative_Characters_FREE/Skeleton_Meshes/SK_Costume_6_001.SK_Costume_6_001")));
	BlackMarketComponent = CreateDefaultSubobject<UACBlackMarketComponent>(TEXT("BlackMarketComponent"));

}

//EACCharacterType AACBlackMarketDealer::GetCharacterType()
//{
//	return EACCharacterType::BlackMarketDealer;
//}

void AACBlackMarketDealer::OnInteract(AACCharacter* ACPlayer, EInteractionKey InKey)
{
	if (ACPlayer->GetCharacterType() == EACCharacterType::Police)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("시민 신분증!"));
	}
	else if (ACPlayer->GetCharacterType() == EACCharacterType::Mafia)
	{
		if (BlackMarketComponent == nullptr)
		{
			return;
		}
		AACMafiaCharacter* MafiaChar = Cast<AACMafiaCharacter>(ACPlayer);
		if (MafiaChar == nullptr)
		{
			return;
		}

		AACMainPlayerController* PC = Cast<AACMainPlayerController>(ACPlayer->GetController());
		if (PC == nullptr)
		{
			AC_LOG(LogSY, Log, TEXT("PC is nullptr"));
			return;
		}

		if (MafiaChar->GetContraband() <= 0)
		{
			PC->ShowNotification(FText::FromString(TEXT("거래할 밀수품이 없습니다.")));
			return;
		}
		PC->ShowNotification(FText::FromString(TEXT("폭탄을 획득했습니다.\n 은행에 폭탄을 설치하세요.")));
		BlackMarketComponent->OpenBlackMarket(ACPlayer);
	}
}

EACInteractorType AACBlackMarketDealer::GetInteractorType() const
{
	return EACInteractorType::BlackMarketDealer;
}

