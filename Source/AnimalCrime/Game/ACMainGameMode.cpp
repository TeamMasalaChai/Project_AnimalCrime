

#include "ACMainGameMode.h"
#include "ACMainPlayerController.h"
#include "Character/ACCharacter.h"
#include "Character/ACTestMafiaCharacter.h"
AACMainGameMode::AACMainGameMode()
{
	PlayerControllerClass = AACMainPlayerController::StaticClass();
	//DefaultPawnClass = AACCharacter::StaticClass();
	DefaultPawnClass = AACTestMafiaCharacter::StaticClass();
}
