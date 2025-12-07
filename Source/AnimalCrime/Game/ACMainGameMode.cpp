

#include "ACMainGameMode.h"
#include "ACMainPlayerController.h"
#include "Character/ACCharacter.h"
AACMainGameMode::AACMainGameMode()
{
	PlayerControllerClass = AACMainPlayerController::StaticClass();
	DefaultPawnClass = AACCharacter::StaticClass();
}
