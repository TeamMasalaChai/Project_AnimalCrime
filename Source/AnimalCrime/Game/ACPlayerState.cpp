
#include "Game/ACPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "AnimalCrime.h"

void AACPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AACPlayerState, CharacterType);
}

void AACPlayerState::OnRep_CharacterType()
{
    AC_LOG(LogSY, Log, TEXT("CharacterType replicated: %d"), (int32)CharacterType);
}
