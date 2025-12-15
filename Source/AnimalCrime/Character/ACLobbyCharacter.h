
#pragma once

#include "CoreMinimal.h"
#include "Character/ACCharacter.h"
#include "ACLobbyCharacter.generated.h"


UCLASS()
class ANIMALCRIME_API AACLobbyCharacter : public AACCharacter
{
	GENERATED_BODY()

public:
	AACLobbyCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
protected:


	virtual void SetSteamFriendsList(const FInputActionValue& Value);

	virtual void SettingsClose(const FInputActionValue& Value) override;

	virtual void GameReady(const FInputActionValue& Value);


protected:
 //!< 스팀 친구 초대 창
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SteamFriendListAction;

 //!< 게임 준비
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ReadyAction;
};
