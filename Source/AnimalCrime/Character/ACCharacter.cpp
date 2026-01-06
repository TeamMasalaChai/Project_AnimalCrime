#include "ACCharacter.h"

#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "NavigationSystem.h"
#include "AI/ACCitizenAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Item/ACEscapeMissionBomb.h"
#include "Kismet/GameplayStatics.h"

#include "Component/ACInteractableComponent.h"
#include "ACCharacterAnimInstance.h"
#include "Components/WidgetComponent.h"
#include "UI/Interaction/ACInteractionInfoWidget.h"
#include "Interaction/ACInteractionData.h"
#include "Game/ACInteractionSubsystem.h"
#include "Interaction/ACInteractionDatabase.h"
#include "Prison/ACPrisonBase.h"

#include "AnimalCrime.h"
#include "NetworkMessage.h"

#include "Component/ACShopComponent.h"
#include "UI/Shop/ACShopWidget.h"
#include "Game/ACMainPlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Component/ACMoneyComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Net/UnrealNetwork.h"
#include "Objects/MoneyData.h"

#include "Game/ACPlayerState.h"
#include "Skill/ACSkillData.h"

#include "Sound/SoundBase.h"
#include "UI/ACHUDWidget.h"

AACCharacter::AACCharacter()
{
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	GetCapsuleComponent()->InitCapsuleSize(35.f, 90.0f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));

	//캐릭터 무브먼트
	auto Move = GetCharacterMovement();
	Move->MaxWalkSpeed = 300.f;
	Move->JumpZVelocity = 500.f;
	Move->AirControl = 0.3f;
	Move->bOrientRotationToMovement = true;
	Move->RotationRate = FRotator(0.f, 500.f, 0.f);

	//스켈레탈 메시
	USkeletalMeshComponent* MeshComp = GetMesh();
	MeshComp->SetSkeletalMesh(
		LoadObject<USkeletalMesh>(
			nullptr, TEXT("/Game/Creative_Characters_FREE/Skeleton_Meshes/SK_Body_010.SK_Body_010")));
	MeshComp->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	MeshComp->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	MeshComp->SetReceivesDecals(false);

	static ConstructorHelpers::FClassFinder<UACCharacterAnimInstance> AnimRef(
		TEXT("/Script/Engine.AnimBlueprint'/Game/Project/Character/ABP_ACPlayer.ABP_ACPlayer_C'"));
	if (AnimRef.Succeeded())
	{
		MeshComp->SetAnimInstanceClass(AnimRef.Class);
	}

	HeadMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Head"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> HeadMeshRef(
		TEXT("/Game/Creative_Characters_FREE/Skeleton_Meshes/SK_Hat_049.SK_Hat_049"));
	if (HeadMeshRef.Succeeded() == true)
	{
		HeadMesh->SetSkeletalMesh(HeadMeshRef.Object);
	}
	HeadMesh->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	HeadMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	HeadMesh->SetupAttachment(RootComponent);
	HeadMesh->SetLeaderPoseComponent(MeshComp);
	HeadMesh->SetReceivesDecals(false);

	FaceMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Face"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> FaceMeshRef(
		TEXT("/Game/Creative_Characters_FREE/Skeleton_Meshes/SK_Male_emotion_usual_001.SK_Male_emotion_usual_001"));
	if (FaceMeshRef.Succeeded() == true)
	{
		FaceMesh->SetSkeletalMesh(FaceMeshRef.Object);
	}
	FaceMesh->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	FaceMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	FaceMesh->SetupAttachment(RootComponent);
	FaceMesh->SetLeaderPoseComponent(MeshComp);
	FaceMesh->SetReceivesDecals(false);

	TopMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Top"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> TopMeshRef(
		TEXT("/Game/Creative_Characters_FREE/Skeleton_Meshes/SK_Outerwear_036.SK_Outerwear_036"));
	if (TopMeshRef.Succeeded() == true)
	{
		TopMesh->SetSkeletalMesh(TopMeshRef.Object);
	}
	TopMesh->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	TopMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	TopMesh->SetupAttachment(RootComponent);
	TopMesh->SetLeaderPoseComponent(MeshComp);
	TopMesh->SetReceivesDecals(false);

	BottomMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Bottom"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> BottomMeshRef(
		TEXT("/Game/Creative_Characters_FREE/Skeleton_Meshes/SK_Pants_014.SK_Pants_014"));
	if (BottomMeshRef.Succeeded() == true)
	{
		BottomMesh->SetSkeletalMesh(BottomMeshRef.Object);
	}
	BottomMesh->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	BottomMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	BottomMesh->SetupAttachment(RootComponent);
	BottomMesh->SetLeaderPoseComponent(MeshComp);
	BottomMesh->SetReceivesDecals(false);

	ShoesMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Shoes"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> ShoesMeshRef(
		TEXT("/Game/Creative_Characters_FREE/Skeleton_Meshes/SK_Shoe_Slippers_005.SK_Shoe_Slippers_005"));
	if (ShoesMeshRef.Succeeded() == true)
	{
		ShoesMesh->SetSkeletalMesh(ShoesMeshRef.Object);
	}
	ShoesMesh->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	ShoesMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	ShoesMesh->SetupAttachment(RootComponent);
	ShoesMesh->SetLeaderPoseComponent(MeshComp);
	ShoesMesh->SetReceivesDecals(false);

	FaceAccMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FaceAcc"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> FaceAccMeshRef(
		TEXT("/Game/Creative_Characters_FREE/Skeleton_Meshes/SK_Moustache_002.SK_Moustache_002"));
	if (FaceAccMeshRef.Succeeded() == true)
	{
		FaceAccMesh->SetSkeletalMesh(FaceAccMeshRef.Object);
	}
	FaceAccMesh->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	FaceAccMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	FaceAccMesh->SetupAttachment(RootComponent);
	FaceAccMesh->SetLeaderPoseComponent(MeshComp);
	FaceAccMesh->SetReceivesDecals(false);

	//카메라
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->TargetOffset = FVector(0.f, 60.f, 20.f); //솔더뷰, Y축으로 오른쪽으로 50

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	
	
	GunSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("GunSpringArm"));
	GunSpringArm->SetupAttachment(RootComponent);
	GunSpringArm->TargetArmLength = 0.0;
	GunSpringArm->bUsePawnControlRotation = true;
	GunSpringArm->SetRelativeLocation(FVector(0.f, 20.f, 40.f));
	
	GunCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("GunCamera"));
	GunCamera->SetupAttachment(GunSpringArm, USpringArmComponent::SocketName);
	GunCamera->bUsePawnControlRotation = false;
	
	InteractBoxComponent = CreateDefaultSubobject<UACInteractableComponent>(TEXT("InteractBoxComponent"));
	InteractBoxComponent->SetupAttachment(RootComponent);

	// 상호작용 위젯 컴포넌트
	InteractionWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
	InteractionWidgetComponent->SetupAttachment(RootComponent);
	InteractionWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	InteractionWidgetComponent->SetDrawSize(FVector2D(300.0f, 100.0f));
	InteractionWidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f)); // 머리 위
	InteractionWidgetComponent->SetVisibility(false); // 기본 숨김

	// 상호작용 위젯 클래스 설정
	static ConstructorHelpers::FClassFinder<UACInteractionInfoWidget> InteractionWidgetRef(
		TEXT("/Game/Project/UI/Interaction/WBP_InteractionInfo.WBP_InteractionInfo_C"));
	if (InteractionWidgetRef.Succeeded())
	{
		InteractionInfoWidgetClass = InteractionWidgetRef.Class;
	}

	// 애니메이션 몽타주
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MeleeMontageRef(
		TEXT("/Game/Project/Character/AM_Melee.AM_Melee"));
	if (MeleeMontageRef.Succeeded())
	{
		MeleeMontage = MeleeMontageRef.Object;
	}

	// ShopComponent 생성
	ShopComponent = CreateDefaultSubobject<UACShopComponent>(TEXT("ShopComponent"));

	GetCharacterMovement()->NetworkSmoothingMode = ENetworkSmoothingMode::Disabled;
	MoneyComp = CreateDefaultSubobject<UACMoneyComponent>(TEXT("MoneyComponent"));


	bReplicates = true;

	// 방망이 휘두르기 사운드 로드
	static ConstructorHelpers::FObjectFinder<USoundBase> BatSwingSoundRef(
		TEXT("/Game/Project/SFX/422506__nightflame__swinging-staff-whoosh-strong-01")
	);
	if (BatSwingSoundRef.Succeeded())
	{
		BatSwingSound = BatSwingSoundRef.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UACSkillData> SkillDataAssetRef(TEXT("/Script/AnimalCrime.ACSkillData'/Game/Project/Character/DA_SkillData.DA_SkillData'"));
	if (SkillDataAssetRef.Succeeded())
	{
		SkillDataAsset = SkillDataAssetRef.Object;
	}
}

#pragma region 엔진 제공 함수

void AACCharacter::PostInitializeComponents()
{
	AC_LOG(LogHY, Error, TEXT("Begin"));
	Super::PostInitializeComponents();
	
	// 캐릭터 스킬
	SprintMoveSpeedData			= SkillDataAsset->SprintMoveSpeed;
	OriginMafiaMoveSpeedData	= SkillDataAsset->OriginMafiaMoveSpeed;
	OriginPoliceMoveSpeedData	= SkillDataAsset->OriginPoliceMoveSpeed;
	SprintGaugeData				= SkillDataAsset->SprintGauge;
	DashForwardImpulseData		= SkillDataAsset->DashForwardImpulse;
	DashUpwardImpulseData		= SkillDataAsset->DashUpwardImpulse;
	DashCoolTimeData			= SkillDataAsset->DashCoolTime;
	
	AC_LOG(LogHY, Error, TEXT("End"));
}

void AACCharacter::BeginPlay()
{
	AC_LOG(LogHY, Error, TEXT("Begin"));
	Super::BeginPlay();

	
	CharacterState = ECharacterState::Free;

	// @Todo 변경 필요. Mafia와 Police 구분이 안감.
	// Police와 Mafia는 각자의 BeginPlay에서 초기화
	// ※ 얘도 확인 했으면 지워주세요
	//MoneyComp->InitMoneyComponent(EMoneyType::MoneyMafiaType);

	AC_LOG(LogHY, Error, TEXT("End"));
}

void AACCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 캐릭터 상태
	DOREPLIFETIME(AACCharacter, CharacterState);
	
	// Skeletal Mesh
	DOREPLIFETIME(AACCharacter, HeadMeshReal);
	DOREPLIFETIME(AACCharacter, FaceMeshReal);
	DOREPLIFETIME(AACCharacter, TopMeshReal);
	DOREPLIFETIME(AACCharacter, BottomMeshReal);
	DOREPLIFETIME(AACCharacter, ShoesMeshReal);
	DOREPLIFETIME(AACCharacter, FaceAccMeshReal);
	
	// 나중에 총 객체 내부로 변경해야 함.
	DOREPLIFETIME(AACCharacter, BulletCount);
}

void AACCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	AC_LOG(LogHY, Warning, TEXT("AACCharacter::EndPlay %s"), *GetName());
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	Super::EndPlay(EndPlayReason);
}

#pragma endregion

void AACCharacter::ChangeInputMode(EInputMode NewMode)
{
	AACMainPlayerController* PC = Cast<AACMainPlayerController>(GetController());
	if (PC)
	{
		PC->ChangeInputMode(NewMode);
	}
}

void AACCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void AACCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

// 기본값 0 (E 키)
void AACCharacter::InteractStarted(int32 InputIndex)
{
	if (!FocusedInteractable || !IsValid(FocusedInteractable))
	{
		UpdateFocus();
		return;
	}

	IACInteractInterface* Interactable = Cast<IACInteractInterface>(FocusedInteractable);
	if (!Interactable)
		return;

	// PrisonDoor 예외: 문이 열려있으면 리턴
	if (Interactable->GetInteractorType() == EACInteractorType::PrisonDoor)
	{
		AACPrisonBase* Prison = Cast<AACPrisonBase>(FocusedInteractable);
		if (Prison && Prison->IsDoorOpen())
			return;
	}

	// InputIndex로 상호작용 선택 (E=0, R=1, T=2)
	if (!FocusedInteractions.IsValidIndex(InputIndex))
	{
		AC_LOG(LogSW, Warning, TEXT("No interaction at index %d"), InputIndex);
		return;
	}

	UACInteractionData* InteractionData = FocusedInteractions[InputIndex];
	if (!InteractionData)
		return;


	RequiredHoldTime = InteractionData->HoldDuration;
	const EInteractionKey Key = static_cast<EInteractionKey>(InputIndex);

	// 즉시 상호작용
	if (RequiredHoldTime <= KINDA_SMALL_NUMBER)
	{
		ServerInteract(FocusedInteractable, Key);
	}
	// 홀드 상호작용
	else
	{
		bIsHoldingInteract = true;
		CurrentHoldTarget = FocusedInteractable;
		CurrentHoldTime = 0.f;
		CurrentHoldKey = Key;
		SetCharacterState(ECharacterState::OnInteract);

		ACharacter* TargetChar = Cast<ACharacter>(CurrentHoldTarget);
		if (TargetChar)
			ServerFreezeCharacter(TargetChar, true);

		AACMainPlayerController* PC = Cast<AACMainPlayerController>(GetController());
		if (PC)
			PC->ShowInteractProgress(InteractionData->InteractionName.ToString());
	}
}

void AACCharacter::InteractHolding(const float DeltaTime)
{
	if (bIsHoldingInteract == false)
	{
		//ResetHoldInteract();
		return;
	}
	//todo: isvalid?
	if (CurrentHoldTarget == nullptr)
	{
		InteractReleased();
		return;
	}
	if (NearInteractables.Contains(CurrentHoldTarget) == false)
	{
		InteractReleased();
		return;
	}


	// 시간 누적
	CurrentHoldTime += DeltaTime;
	//AC_LOG(LogSW, Log, TEXT("%s Holding at %f"), *CurrentHoldTarget->GetName(), CurrentHoldTime);

	// UI 업데이트
	AACMainPlayerController* PC = Cast<AACMainPlayerController>(GetController());
	if (PC != nullptr)
	{
		PC->UpdateInteractProgress(GetHoldProgress());
	}

	// 홀드 시간 완료 시 상호작용 실행
	if (CurrentHoldTime >= RequiredHoldTime)
	{
		ServerInteract(CurrentHoldTarget, CurrentHoldKey);
		InteractReleased();
	}
}

void AACCharacter::InteractReleased()
{
	if (bIsHoldingInteract == false)
	{
		return;
	}

	// UI 표시
	AACMainPlayerController* PC = Cast<AACMainPlayerController>(GetController());
	if (PC != nullptr)
	{
		//UE_LOG(LogSW, Log, TEXT("Hide start"));
		PC->HideInteractProgress();
	}

	ResetHoldInteract();
}

void AACCharacter::ItemDrop()
{
	UE_LOG(LogTemp, Log, TEXT("ItemDrop!!"));
}

void AACCharacter::Attack()
{
	if (CharacterState == ECharacterState::OnInteract || CharacterState == ECharacterState::Stun || CharacterState == ECharacterState::Prison)
	{
		return;
	}
	// 현재 공격 중인지 확인. 
	if (CheckProcessAttack() == true)
	{
		return;
	}

	// Listen Server에 존재하는 클라이언트 경우
	if (HasAuthority())
	{
		PerformAttackTrace();
	}
	// Listen Server에 접속한 클라이언트 경우
	else
	{
		ServerAttack();
	}
}

void AACCharacter::SettingsClose()
{
	//자식에서 추가로 구현
	switch (SettingMode)
	{
	case ESettingMode::None:
		break;
	case ESettingMode::Default:
		break;
	default:
		break;
	}
}

#pragma region 캐릭터 스킬 - Dash(1)

void AACCharacter::Dash(const FInputActionValue& Value)
{
	bool bDashFlag = Value.Get<bool>();
	AC_LOG(LogHY, Log, TEXT("PlayerController -> AACCharacter | Input: %d"), bDashFlag);
	
	// Case: 심각한 에러 - 나와서는 안되는 값.
	if (bDashFlag == false)
	{
		AC_LOG(LogHY, Error, TEXT("Dash Input False"));
		return;
	}

	// Server에게 Dash Skill 사용 요청
	ServerDash();
}

void AACCharacter::ServerDash_Implementation()
{
	// DashTimerHandle이 살아있는 경우는 실행 불가.
	if (GetWorldTimerManager().IsTimerActive(DashTimerHandle) == true)
	{
		AC_LOG(LogHY, Error, TEXT("Dash Timer is already active"));
		return;
	}
	
	bDashCoolDown = false;
	
	AC_LOG(LogHY, Error, TEXT("Excute [Server Dash]"));

	// 캐릭터가 바라보는 방향 획득
	FVector ForwardDir = GetActorForwardVector();
	
	// 날아가는 힘 계산
	FVector LaunchVelocity = ForwardDir * DashForwardImpulseData + FVector(0.f, 0.f, DashUpwardImpulseData);
	LaunchCharacter(LaunchVelocity, true, false);
	
	// Dash 사용 쿨타임 적용.
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUObject(this, &AACCharacter::ResetDashFlag);
	GetWorld()->GetTimerManager().SetTimer(DashTimerHandle, TimerDelegate, DashCoolTimeData, false);
}

void AACCharacter::ResetDashFlag()
{
	bDashCoolDown = true;
}

#pragma endregion

#pragma region 캐릭터 스킬 - Sprint(2)

void AACCharacter::Sprint(const FInputActionValue& Value)
{
	if (Value.Get<bool>())
	{
		if (SprintGauge > 0)
		{
			ServerSprintStart();
		}
	}
	else
	{
		ServerSprintEnd();
	}
}

void AACCharacter::ServerSprintStart_Implementation()
{
	if (bSprint == true)
	{
		AC_LOG(LogHY, Error, TEXT("Sprint Timer is already active"));
		return;
	}

	// Delete Guage Up 
	GetWorld()->GetTimerManager().ClearTimer(SprintGaugeUpTimerHandle);

	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUObject(this, &AACCharacter::GaugeDown);

	GetWorld()->GetTimerManager().SetTimer(SprintGaugeDownTimerHandle, TimerDelegate, 1, true);

	bSprint = true;
	OnRep_Sprint();
}

void AACCharacter::ServerSprintEnd_Implementation()
{
	if (bSprint == false)
	{
		AC_LOG(LogHY, Error, TEXT("Sprint Timer is already deactive"));
		// return은 필요 없음.
	}
	
	// 게이지 Down에 대한 Timer 종료
	GetWorld()->GetTimerManager().ClearTimer(SprintGaugeDownTimerHandle);

	// 게이지 Up에 대한 Timer 시작
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUObject(this, &AACCharacter::GaugeUp);
	GetWorld()->GetTimerManager().SetTimer(SprintGaugeUpTimerHandle, TimerDelegate, 1, true);

	bSprint = false;
	OnRep_Sprint();
}

void AACCharacter::OnRep_Sprint()
{
	if (bSprint)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintMoveSpeedData;
		// AC_LOG(LogHY, Warning, TEXT("Sprint is active %f"), GetCharacterMovement()->MaxWalkSpeed);
	}
	else
	{
		if (GetCharacterType() == EACCharacterType::Mafia)
		{
			GetCharacterMovement()->MaxWalkSpeed = OriginMafiaMoveSpeedData;
		}
		else if (GetCharacterType() == EACCharacterType::Police)
		{
			GetCharacterMovement()->MaxWalkSpeed = OriginPoliceMoveSpeedData;
		}
	}
}

void AACCharacter::GaugeUp()
{
	// 1씩 증가
	SprintGauge += 1;
	// AC_LOG(LogHY, Warning, TEXT("Gauge Up: %d"), SprintGauge);
	if (SprintGauge > SprintGaugeData)
	{
		SprintGauge = SprintGaugeData;
	}
}

void AACCharacter::GaugeDown()
{
	SprintGauge -= 1;
	// AC_LOG(LogHY, Warning, TEXT("Gauge Down: %d"), SprintGauge);
	if (SprintGauge <= 0)
	{
		GetWorldTimerManager().ClearTimer(SprintGaugeDownTimerHandle);
		bSprint = false;
		OnRep_Sprint();
	}
}

#pragma endregion

void AACCharacter::Jump()
{
	// Case: 스턴 및 감옥 상태일 경우 Jump 불가 
	if (CharacterState == ECharacterState::Stun ||
		CharacterState == ECharacterState::Prison)
	{
		return;
	}

	Super::Jump();
}

void AACCharacter::ServerInteract_Implementation(AActor* Target, EInteractionKey InKey)
{
	if (Target == nullptr)
	{
		AC_LOG(LogSW, Log, TEXT("NULL!!"));
		return;
	}

	if (IsValid(Target) == false)
	{
		AC_LOG(LogSW, Log, TEXT("Interactable Deleted!!"));
		return;
	}

	IACInteractInterface* Interactable = Cast<IACInteractInterface>(Target);
	//if (Interactable == nullptr)
	//{                                                                                                     
	//    AC_LOG(LogSW, Log, TEXT("%s has No Interface!!"), *Target->GetName());                            
	//    return;                                                                                           
	//}                                                                                                     

	//// 상호작용 가능한지 서버에서 확실하게 체크
	//EACInteractorType TargetType = Interactable->GetInteractorType();                                     

	//// PrisonDoor 예외: 문이 열려있으면 리턴                                                                  
	//if (TargetType == EACInteractorType::PrisonDoor)                                                      
	//{                                                                                                     
	//    AACPrisonBase* Prison = Cast<AACPrisonBase>(Target);                                              
	//    if (Prison && Prison->IsDoorOpen())                                                               
	//    {                                                                                                 
	//        AC_LOG(LogSW, Log, TEXT("Prison door is already open!"));                                     
	//        return;                                                                                       
	//    }                                                                                                 
	//}                                                                                                     

	//// DB에서 상호작용 가능 여부 체크                                                                         
	//UACInteractionSubsystem* InteractionSys = GetGameInstance()->GetSubsystem<UACInteractionSubsystem>(); 
	//UACInteractionDatabase* DB = InteractionSys ? InteractionSys->GetInteractionDatabase() : nullptr;     
	//if (!DB || DB->GetInteractions(GetCharacterType(), TargetType).Num() == 0)                            
	//{                                                                                                     
	//    AC_LOG(LogSW, Log, TEXT("%s Cant Interact with %s!!"), *Target->GetName(), *GetName());           
	//    return;                                                                                           
	//

	Interactable->OnInteract(this, InKey);
	AC_LOG(LogSW, Log, TEXT("%s Interacted with %s!!"), *Target->GetName(), *GetName());
}

bool AACCharacter::CheckProcessAttack() const
{
	return bAttackFlag;
}

void AACCharacter::ChangeAttackTrue()
{
	bAttackFlag = true;
}

void AACCharacter::ChangeAttackFalse()
{
	bAttackFlag = false;
}

void AACCharacter::PerformAttackTrace()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance == nullptr)
	{
		return;
	}

	// Todo: 멀티캐스트에서 변경하기
	MulticastPlayAttackMontage();
}

void AACCharacter::AttackHitCheck()
{
	// Empty Class
	// 자식 클래스에서 구현
}

void AACCharacter::FireHitscan()
{
	UACItemData* EquippedWeapon = ShopComponent->EquippedWeapon;
	if (EquippedWeapon == nullptr)
	{
		AC_LOG(LogHY, Log, TEXT("EquippedWeapon is nullptr"));
		return;
	}
	if (EquippedWeapon->ItemType != EItemType::Weapon)
	{
		AC_LOG(LogTemp, Log, TEXT(">>> Weapon Equipped: %s"), *EquippedWeapon->ItemName.ToString());
		return;
	}
	if (EquippedWeapon->ItemName.ToString() != TEXT("Pistol_001"))
    {
		AC_LOG(LogTemp, Log, TEXT(">>>	 Weapon Equipped: %s"), *EquippedWeapon->ItemName.ToString());
		return;
	}
	
	if (GetBulletCount() <= 0)
	{
		AC_LOG(LogTemp, Log, TEXT("총알 갯수 %d"), GetBulletCount());
		return;
	}
	
	ServerShoot();
}

void AACCharacter::SetCarryState(bool bPlay)
{
	if (HasAuthority())
	{
		Multicast_SetCarryState(bPlay);
	}
	else
	{
		Server_SetCarryState(bPlay);
	}
}

void AACCharacter::Server_SetCarryState_Implementation(bool bPlay)
{
	Multicast_SetCarryState(bPlay);
}

void AACCharacter::Multicast_SetCarryState_Implementation(bool bPlay)
{
	UACCharacterAnimInstance* Anim = Cast<UACCharacterAnimInstance>(GetMesh()->GetAnimInstance());
	if (Anim == nullptr)
	{
		return;
	}

	Anim->SetIsCarrying(bPlay);
}

void AACCharacter::OnInteract(AACCharacter* ACPlayer, EInteractionKey InKey)
{
}

EACInteractorType AACCharacter::GetInteractorType() const
{
	return EACInteractorType::Citizen;
}

UWidgetComponent* AACCharacter::GetInteractionWidget() const
{
	return InteractionWidgetComponent;
}

void AACCharacter::ShowInteractionHints(const TArray<UACInteractionData*>& Interactions)
{
	if (!InteractionWidgetComponent) return;

	// 위젯 생성 (최초 1회)
	if (!InteractionWidgetComponent->GetWidget() && InteractionInfoWidgetClass)
	{
		InteractionWidgetComponent->SetWidgetClass(InteractionInfoWidgetClass);
	}

	UACInteractionInfoWidget* HintWidget = Cast<UACInteractionInfoWidget>(
		InteractionWidgetComponent->GetWidget());
	if (!HintWidget) return;

	HintWidget->UpdateInteractions(Interactions);
	HintWidget->ShowWidget();
	InteractionWidgetComponent->SetVisibility(true);
}

void AACCharacter::HideInteractionHints()
{
	if (!InteractionWidgetComponent) return;

	UACInteractionInfoWidget* HintWidget = Cast<UACInteractionInfoWidget>(
		InteractionWidgetComponent->GetWidget());
	if (HintWidget)
		HintWidget->HideWidget();

	InteractionWidgetComponent->SetVisibility(false);
}

void AACCharacter::UpdateFocus()
{
	// 로컬 플레이어만 상호작용 UI 표시
	if (!IsLocallyControlled())
	{
		return;
	}

	AActor* PreviousFocus = FocusedInteractable;
	FocusedInteractable = nullptr;
	//FocusedInteractions.Empty();

	// 1. 거리순 정렬
	if (SortNearInteractables() == false)
	{
		// 주변에 상호작용 가능한 객체 없음 - 이전 Focus 위젯 숨김
		if (PreviousFocus != nullptr)
		{
			IACInteractInterface* PrevInteractable = Cast<IACInteractInterface>(PreviousFocus);
			if (PrevInteractable)
				PrevInteractable->HideInteractionHints();
		}
		return;
	}

	// 2. 첫 번째 유효한 액터 찾기
	UACInteractionSubsystem* InteractionSys = GetGameInstance()->GetSubsystem<UACInteractionSubsystem>();
	UACInteractionDatabase* DB = InteractionSys ? InteractionSys->GetInteractionDatabase() : nullptr;

	EACCharacterType CharacterType = GetCharacterType();
	IACInteractInterface* FocusedInterface = nullptr;
	EACInteractorType FocusedTargetType = EACInteractorType::Citizen;

	for (AActor* Candidate : NearInteractables)
	{
		if (!IsValid(Candidate)) continue;

		IACInteractInterface* Interactable = Cast<IACInteractInterface>(Candidate);
		if (!Interactable) continue;

		EACInteractorType TargetType = Interactable->GetInteractorType();

		// DB에 상호작용이 없으면 스킵
		if (!DB || !DB->HasInteractions(CharacterType, TargetType))
			continue;

		// PrisonDoor 예외: 문이 열려있으면 스킵
		if (TargetType == EACInteractorType::PrisonDoor)
		{
			AACPrisonBase* Prison = Cast<AACPrisonBase>(Candidate);
			if (Prison && Prison->IsDoorOpen())
				continue;
		}

		FocusedInteractable = Candidate;
		FocusedInterface = Interactable;
		FocusedTargetType = TargetType;
		break;
	}

	// 3. Focus 변경 시 위젯 업데이트
	if (FocusedInteractable != PreviousFocus)
	{
		// 이전 Focus 위젯 숨김
		if (PreviousFocus)
		{
			IACInteractInterface* PrevInteractable = Cast<IACInteractInterface>(PreviousFocus);
			if (PrevInteractable)
				PrevInteractable->HideInteractionHints();
		}

		// 새 Focus - 이미 가져온 DB로 상호작용 조회 및 위젯 표시
		if (FocusedInteractable && FocusedInterface && DB)
		{
			FocusedInteractions.Empty();
			TArray<UACInteractionData*> AllInteractions = DB->GetInteractions(CharacterType, FocusedTargetType);

			for (UACInteractionData* InteractionData : AllInteractions)
			{
				if (!InteractionData) continue;

				// 캐릭터 상호작용인 경우만 상태 체크
				//if (InteractionData->bIsCharacterInteraction)
				//{
				//	AACCharacter* TargetChar = Cast<AACCharacter>(FocusedInteractable);
				//	if (TargetChar)
				//	{
				//		if (InteractionData->InteractableState != TargetChar->CharacterState)
				//			continue; // 상태 불일치 - 필터링
				//	}
				//}

				FocusedInteractions.Add(InteractionData);
			}

			FocusedInterface->ShowInteractionHints(FocusedInteractions);
		}
	}
}

void AACCharacter::QueryInteractionsForFocus()
{
	FocusedInteractions.Empty();

	// 1. Subsystem 가져오기
	UACInteractionSubsystem* InteractionSys =
		GetGameInstance()->GetSubsystem<UACInteractionSubsystem>();
	if (!InteractionSys) return;

	UACInteractionDatabase* DB = InteractionSys->GetInteractionDatabase();
	if (!DB) return;

	// 2. 타입 정보 가져오기
	EACCharacterType InitiatorType = GetCharacterType();
	IACInteractInterface* Interactable = Cast<IACInteractInterface>(FocusedInteractable);
	if (!Interactable) return;

	EACInteractorType TargetType = Interactable->GetInteractorType();

	// 3. DB 조회
	TArray<UACInteractionData*> AllInteractions = DB->GetInteractions(InitiatorType, TargetType);
	AC_LOG(LogSW, Log, TEXT("Interaction Count: %d"), AllInteractions.Num())

		// 4. 캐릭터 상태로 필터링 (bIsCharacterInteraction 사용)
		for (UACInteractionData* InteractionData : AllInteractions)
		{
			if (!InteractionData) continue;

			// 캐릭터 상호작용인 경우만 상태 체크
			//if (InteractionData->bIsCharacterInteraction)
			//{
			//	AACCharacter* TargetChar = Cast<AACCharacter>(FocusedInteractable);
			//	if (TargetChar)
			//	{
			//		if (InteractionData->InteractableState != TargetChar->CharacterState)
			//			continue; // 상태 불일치 - 필터링
			//	}
			//}

			FocusedInteractions.Add(InteractionData);
		}

	// 5. 위젯에 상호작용 정보 전달
	Interactable->ShowInteractionHints(FocusedInteractions);
}

void AACCharacter::AddInteractable(AActor* Interactor)
{
	//ensureAlways(Interactor);
	NearInteractables.AddUnique(Interactor);
	UpdateFocus();
}

void AACCharacter::RemoveInteractable(AActor* Interactor)
{
	//ensureAlways(Interactor);
	NearInteractables.Remove(Interactor);
	UpdateFocus();
}

bool AACCharacter::SortNearInteractables()
{
	for (int32 i = NearInteractables.Num() - 1; i >= 0; --i)
	{
		if (IsValid(NearInteractables[i]) == true)
		{
			continue;
		}

		NearInteractables.RemoveAt(i);
	}

	if (NearInteractables.Num() == 0)
	{
		AC_LOG(LogSW, Log, TEXT("No Close Interactables!!"));
		return false;
	}

	if (NearInteractables.Num() == 1)
	{
		//AC_LOG(LogSW, Log, TEXT("NO NEED TO SORT!!"));
		return true;
	}

	const FVector PlayerLocation = GetActorLocation();
	NearInteractables.Sort([PlayerLocation](const AActor& A, const AActor& B)
		{
			float DistA = FVector::DistSquared(PlayerLocation, A.GetActorLocation());
			float DistB = FVector::DistSquared(PlayerLocation, B.GetActorLocation());
			return DistA < DistB;
		});

	return true;
}

void AACCharacter::OnRep_CharacterState()
{
	//if (IsValid(this) == false)
	//{
	//	return;
	//}		

	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (IsValid(this) == false)
	{
		AC_LOG(LogHY, Error, TEXT("this is Invalid"));
		return;
	}

	if (MoveComp == nullptr)
	{
		AC_LOG(LogHY, Error, TEXT("MoveComp is nullptr"));
		return;
	}

	AC_LOG(LogHY, Error, TEXT("CharacterType:%d name:%s"), CharacterState, *GetName());

	switch (CharacterState)
	{
	case ECharacterState::OnInteract:
	{
		// MoveComp->SetMovementMode(MOVE_None);
		break;
	}
	case ECharacterState::Stun:
	{
		MoveComp->MaxWalkSpeed = 10.f;
		MoveComp->JumpZVelocity = 0.f;
		break;
	}
	case ECharacterState::Free:
	{
		// MoveComp->SetMovementMode(MOVE_Walking);
		if (GetCharacterType() == EACCharacterType::Police)
		{
			MoveComp->MaxWalkSpeed = 500.0f; // 경찰
		}
		else
		{
			MoveComp->MaxWalkSpeed = 300.0f; // 마피아/시민
		}
		MoveComp->JumpZVelocity = 500.0f;
		break;
	}
	case ECharacterState::OnDamage:
	{
		MoveComp->MaxWalkSpeed = 600.0f;
		break;
	}
	}

	//// Focus된 캐릭터의 상태가 변경되면 상호작용 재계산
	//if (GetWorld())
	//{
	//	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	//	{
	//		AACMainPlayerController* PC = Cast<AACMainPlayerController>(*It);
	//		if (!PC) continue;

	//		AACCharacter* LocalChar = PC->GetPawn<AACCharacter>();
	//		if (!LocalChar) continue;

	//		// 로컬 플레이어가 이 캐릭터를 Focus 중이면 상호작용 재계산
	//		if (LocalChar->FocusedInteractable == this)
	//		{
	//			LocalChar->QueryInteractionsForFocus();
	//		}
	//	}
	//}
}

ECharacterState AACCharacter::GetCharacterState() const
{
	return CharacterState;
}

void AACCharacter::SetCharacterState(ECharacterState InCharacterState)
{
	CharacterState = InCharacterState;
}

void AACCharacter::ResetHoldInteract()
{
	if (CurrentHoldTarget == nullptr)
	{
		return;
	}

	// Server RPC로 대상 움직임 재개 (ACharacter 사용 - 시민도 포함)
	ACharacter* TargetChar = Cast<ACharacter>(CurrentHoldTarget);
	if (TargetChar != nullptr)
	{
		ServerFreezeCharacter(TargetChar, false);
	}

	// todo: 임시로 홀드 상호작용 리셋 시 다시 콜리전 오버랩 시작해야함
	RemoveInteractable(CurrentHoldTarget);
	UpdateFocus();

	bIsHoldingInteract = false;
	CurrentHoldTarget = nullptr;
	CurrentHoldTime = 0.f;
	RequiredHoldTime = 0.f;
	SetCharacterState(ECharacterState::Free);
}

float AACCharacter::GetHoldProgress() const
{
	if (RequiredHoldTime <= 0.f)
	{
		return 0.f;
	}

	return FMath::Clamp(CurrentHoldTime / RequiredHoldTime, 0.f, 1.f);
}

void AACCharacter::MulticastPlayAttackMontage_Implementation()
{
	if (MeleeMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(MeleeMontage, 2.0f);
	}

	// 로컬 클라이언트 + 무기 장착 시에만 사운드 재생
	if (IsLocallyControlled() && BatSwingSound && ShopComponent)
	{
		UACItemData* EquippedWeapon = ShopComponent->EquippedWeapon;

		// 무기를 들고 있는지 확인
		if (EquippedWeapon != nullptr && EquippedWeapon->ItemType == EItemType::Weapon)
		{
			UGameplayStatics::PlaySound2D(this, BatSwingSound);
		}
	}
}

void AACCharacter::ServerAttack_Implementation()
{
	PerformAttackTrace(); // 서버에서만 실행됨
}

void AACCharacter::ServerItemDrop_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("Server ItemDrop!!"));
}

void AACCharacter::ServerSetTargetState_Implementation(AACCharacter* Target, ECharacterState NewState)
{
	if (Target == nullptr)
	{
		return;
	}
	Target->SetCharacterState(NewState);
}

void AACCharacter::ServerFreezeCharacter_Implementation(ACharacter* Target, bool bFreeze)
{
	if (Target == nullptr)
	{
		return;
	}

	UCharacterMovementComponent* MoveComp = Target->GetCharacterMovement();
	if (MoveComp == nullptr)
	{
		return;
	}

	if (bFreeze)
	{
		MoveComp->SetMovementMode(MOVE_None);
	}
	else
	{
		MoveComp->SetMovementMode(MOVE_Walking);
	}
}

EACCharacterType AACCharacter::GetCharacterType()
{
	return EACCharacterType::Citizen;
}

#pragma region Update SkeletalMeshComponent

void AACCharacter::OnRep_HeadMesh() const
{
	if (HeadMesh)
	{
		AC_LOG(LogHY, Error, TEXT("Before HeadMeshComp OK | prev:%s next:%s"), HeadMesh->GetSkeletalMeshAsset() == nullptr ? TEXT("No Asset") : *HeadMesh->GetSkeletalMeshAsset()->GetName(), *HeadMeshReal.GetName());
		UpdateHeadMesh();
		AC_LOG(LogHY, Error, TEXT("After HeadMeshComp OK | prev:%s next:%s"), HeadMesh->GetSkeletalMeshAsset() == nullptr ? TEXT("No Asset") : *HeadMesh->GetSkeletalMeshAsset()->GetName(), *HeadMeshReal.GetName());
	}
}

void AACCharacter::OnRep_FaceMesh() const
{
	if (FaceMesh)
	{
		AC_LOG(LogHY, Error, TEXT("Before FaceMeshComp OK | prev:%s next:%s"), FaceMesh->GetSkeletalMeshAsset() == nullptr ? TEXT("No Asset") : *FaceMesh->GetSkeletalMeshAsset()->GetName(), *FaceMeshReal.GetName());
		UpdateFaceMesh();
		AC_LOG(LogHY, Error, TEXT("After FaceMeshComp OK | prev:%s next:%s"), FaceMesh->GetSkeletalMeshAsset() == nullptr ? TEXT("No Asset") : *FaceMesh->GetSkeletalMeshAsset()->GetName(), *FaceMeshReal.GetName());
	}
}

void AACCharacter::OnRep_TopMesh() const
{
	if (TopMesh)
	{
		AC_LOG(LogHY, Error, TEXT("Before TopMeshComp OK | prev:%s next:%s"), TopMesh->GetSkeletalMeshAsset() == nullptr ? TEXT("No Asset") : *TopMesh->GetSkeletalMeshAsset()->GetName(), *TopMeshReal.GetName());
		UpdateTopMesh();
		AC_LOG(LogHY, Error, TEXT("After TopMeshComp OK | prev:%s next:%s"), TopMesh->GetSkeletalMeshAsset() == nullptr ? TEXT("No Asset") : *TopMesh->GetSkeletalMeshAsset()->GetName(), *TopMeshReal.GetName());
	}
}

void AACCharacter::OnRep_BottomMesh() const
{
	if (BottomMesh)
	{
		AC_LOG(LogHY, Error, TEXT("Before BottomMeshComp OK | prev:%s next:%s"), BottomMesh->GetSkeletalMeshAsset() == nullptr ? TEXT("No Asset") : *BottomMesh->GetSkeletalMeshAsset()->GetName(), *BottomMeshReal.GetName());
		UpdateBottomMesh();
		AC_LOG(LogHY, Error, TEXT("After BottomMeshComp OK | prev:%s next:%s"), BottomMesh->GetSkeletalMeshAsset() == nullptr ? TEXT("No Asset") : *BottomMesh->GetSkeletalMeshAsset()->GetName(), *BottomMeshReal.GetName());
	}
}

void AACCharacter::OnRep_ShoesMesh() const
{
	if (ShoesMesh)
	{
		AC_LOG(LogHY, Error, TEXT("Before ShoesMeshComp OK | prev:%s next:%s"), ShoesMesh->GetSkeletalMeshAsset() == nullptr ? TEXT("No Asset") : *ShoesMesh->GetSkeletalMeshAsset()->GetName(), *ShoesMeshReal.GetName());
		UpdateShoesMesh();
		AC_LOG(LogHY, Error, TEXT("After ShoesMeshComp OK | prev:%s next:%s"), ShoesMesh->GetSkeletalMeshAsset() == nullptr ? TEXT("No Asset") : *ShoesMesh->GetSkeletalMeshAsset()->GetName(), *ShoesMeshReal.GetName());
	}
}

void AACCharacter::OnRep_FaceAccMesh() const
{
	if (FaceAccMesh)
	{
		AC_LOG(LogHY, Error, TEXT("Before ShoesMeshComp OK | prev:%s next:%s"), FaceAccMesh->GetSkeletalMeshAsset() == nullptr ? TEXT("No Asset") : *FaceAccMesh->GetSkeletalMeshAsset()->GetName(), *FaceAccMeshReal.GetName());
		UpdateFaceAccMesh();
		AC_LOG(LogHY, Error, TEXT("After ShoesMeshComp OK | prev:%s next:%s"), FaceAccMesh->GetSkeletalMeshAsset() == nullptr ? TEXT("No Asset") : *FaceAccMesh->GetSkeletalMeshAsset()->GetName(), *FaceAccMeshReal.GetName());
	}
}

#pragma endregion


// #####################################################################################
// ########################## 나중에 사라져야할 코드 ######################################
// #####################################################################################

void AACCharacter::ServerShoot_Implementation()
{
	SpendBullets(1);
	
	UCameraComponent* ActiveCamera = GunCamera;
	
	
	FVector CameraLoc;
	FRotator CameraRot;
	float MaxDistance = 2000;
	GetOwner()->GetActorEyesViewPoint(CameraLoc, CameraRot);
	AC_LOG(LogHY, Log, TEXT("Firing Hitscan!! Who:%s %s"), *CameraLoc.ToString(), *CameraRot.ToString());
	CameraLoc = ActiveCamera->GetComponentLocation();
	CameraRot = ActiveCamera->GetComponentRotation();
	
	AC_LOG(LogHY, Log, TEXT("Firing Hitscan!! Gun:%s %s"), *CameraLoc.ToString(), *CameraRot.ToString());
	AC_LOG(LogHY, Log, TEXT("Firing Hitscan!! Follow:%s %s"), *FollowCamera->GetComponentLocation().ToString(), *FollowCamera->GetComponentRotation().ToString());
	
	FVector TraceEnd = CameraLoc + (CameraRot.Vector() * MaxDistance);

	// 총구 위치
	// 방안 [소켓으로부터 출발] [SkeletalMesh의 경우 거기서]
	//FVector MuzzleLoc = GetMesh()->GetSocketLocation("RightHandSocket");

	FVector MuzzleLoc = CameraLoc;
	FVector ShootDir = (TraceEnd - MuzzleLoc).GetSafeNormal();
	FVector End = MuzzleLoc + (ShootDir * MaxDistance);

	FHitResult Hit;

	FCollisionObjectQueryParams ObjectParams;
	
	ObjectParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectParams.AddObjectTypesToQuery(ECC_GameTraceChannel1);
	ObjectParams.AddObjectTypesToQuery(ECC_GameTraceChannel6);
	ObjectParams.AddObjectTypesToQuery(ECC_GameTraceChannel7);
	ObjectParams.AddObjectTypesToQuery(ECC_GameTraceChannel8);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(GetOwner());
	QueryParams.bReturnPhysicalMaterial = true;
	
	bool bHit = GetWorld()->LineTraceSingleByObjectType(Hit, MuzzleLoc, End, ObjectParams, QueryParams);
	
	FColor LineColor = bHit ? FColor::Red : FColor::Green;
	DrawDebugLine(GetWorld(),MuzzleLoc, bHit ? Hit.ImpactPoint : End, LineColor, false, 2.0f, 0, 2.0f);
	if (bHit)
	{
		UE_LOG(LogTemp, Log, TEXT("Hit: %s"), *Hit.GetActor()->GetName());
		UGameplayStatics::ApplyDamage(Hit.GetActor(),30.0f, GetController(),this, nullptr);
	}
}

int32 AACCharacter::GetBulletCount() const
{
	return BulletCount;
}

void AACCharacter::AddBullets(int32 InBulletCount)
{
	AC_LOG(LogHY, Error, TEXT("Current Bullet Count: %d Input BulletCount: %d"), BulletCount, InBulletCount);
	BulletCount += InBulletCount;
	OnRep_BulletCount();
	// AACMainPlayerController* MainPlayerController = Cast<AACMainPlayerController>(GetController());
	// if (MainPlayerController == nullptr)
	// {
	// 	return;
	// }
	// if (MainPlayerController->ACHUDWidget == nullptr)
	// {
	// 	AC_LOG(LogHY, Log, TEXT("ACHUDWidget is nullptr"));
	// 	return;
	// }
	// MainPlayerController->ACHUDWidget->HandleAmmoChanged(BulletCount);
}

void AACCharacter::ClearBullets()
{
	BulletCount = 0;
	OnRep_BulletCount();
	// AACMainPlayerController* MainPlayerController = Cast<AACMainPlayerController>(GetController());
	// if (MainPlayerController == nullptr)
	// {
	// 	return;
	// }
	// MainPlayerController->ACHUDWidget->HandleAmmoChanged(BulletCount);
}

void AACCharacter::SpendBullets(int32 InBulletCount)
{
	if (InBulletCount <= 0)
	{
		AC_LOG(LogHY, Error, TEXT("Input BulletCount is Zero"));
		return;
	}
	
	if (BulletCount < InBulletCount)
	{
		AC_LOG(LogHY, Error, TEXT("Current Bullet Count: %d Input BulletCount: %d"), BulletCount, InBulletCount);
		return;
	}
	BulletCount -= InBulletCount;
	OnRep_BulletCount();
	
	// AACMainPlayerController* MainPlayerController = Cast<AACMainPlayerController>(GetController());
	// if (MainPlayerController == nullptr)
	// {
	// 	return;
	// }
	// MainPlayerController->ACHUDWidget->HandleAmmoChanged(BulletCount);
}

void AACCharacter::OnRep_BulletCount()
{
	AACMainPlayerController* PC = Cast<AACMainPlayerController>(GetController());
	if (PC && PC->IsLocalController())
	{
		PC->UpdateAmmoUI(BulletCount);
	}
}
