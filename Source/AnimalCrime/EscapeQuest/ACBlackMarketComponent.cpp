#include "EscapeQuest/ACBlackMarketComponent.h"
#include "Item/ACEscapeMissionBomb.h"
#include "Character/ACMafiaCharacter.h"
#include "Game/ACMainGameState.h"
#include "Game/ACPlayerState.h"
#include "Character/ACCharacter.h"
#include "Game/ACMainPlayerController.h"

#include "AnimalCrime.h"

UACBlackMarketComponent::UACBlackMarketComponent()
{

}

void UACBlackMarketComponent::OpenBlackMarket(AACCharacter* InteractingPlayer)
{
	AC_SUBLOG(LogSY, Log, TEXT("BlackMarket Opened"));
	if (InteractingPlayer == nullptr)
	{
		AC_SUBLOG(LogSY, Warning, TEXT("InteractingPlayer is nullptr"));
		return;
	}

	AACMafiaCharacter* Mafia = Cast<AACMafiaCharacter>(InteractingPlayer);
	if (Mafia == nullptr)
	{
		AC_SUBLOG(LogSY, Log, TEXT("Character is not Mafia"));
		return;
	}

	// 서버에서만 실행되도록 보호
	if (GetOwner() == nullptr || GetOwner()->HasAuthority() == false)
	{
		AC_SUBLOG(LogSY, Warning, TEXT("OpenBlackMarket called on non-server"));
		return;
	}

	AACPlayerState* PS = Mafia->GetPlayerState<AACPlayerState>();
	if (PS == nullptr)
	{
		AC_SUBLOG(LogSY, Warning, TEXT("PlayerState is nullptr"));
		return;
	}
	if (PS->EscapeState != EEscapeState::BlackMarket)
	{
		AC_SUBLOG(LogSY, Log, TEXT("EscapeState is not DeliverBomb"));
		return;
	}

	if (Mafia->HandBomb != nullptr)
	{
		AC_SUBLOG(LogSY, Log, TEXT("Already Hand has Bomb"));
	}

	else
	{
		AC_SUBLOG(LogSY, Log, TEXT("Get Bomb!!"));
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		// 폭탄 스폰
		AACEscapeMissionBomb* SpawnedBomb = GetWorld()->SpawnActor<AACEscapeMissionBomb>
			(AACEscapeMissionBomb::StaticClass(),
				InteractingPlayer->GetActorLocation(),
				FRotator::ZeroRotator,
				Params
			);

		SpawnedBomb->AttachedCharacter = Mafia;

		//아이템 핸드로 이동
		Mafia->HandBomb = SpawnedBomb;

		//폭탄 설치 가능 구역 보이게 하기
		Mafia->ClientSetBombHeld(true);
		//캐릭터에 폭탄 부착
		SpawnedBomb->AttachToCharacter();

		PS->EscapeState = EEscapeState::DeliverBomb;
		PS->OnRep_EscapeState();
	}
}


void UACBlackMarketComponent::BeginPlay()
{
	Super::BeginPlay();


}


