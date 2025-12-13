

#include "ACLobbyGameMode.h"
#include "ACLobbyPlayerController.h"
#include "Character/ACTestMafiaCharacter.h"

AACLobbyGameMode::AACLobbyGameMode()
{
	PlayerControllerClass = AACLobbyPlayerController::StaticClass();
	DefaultPawnClass = AACTestMafiaCharacter::StaticClass();
}
