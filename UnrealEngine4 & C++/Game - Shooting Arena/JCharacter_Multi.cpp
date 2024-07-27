#include "GameActor/Pawn/JCharacter_Multi.h"
#include "Net/UnrealNetwork.h"

AJCharacter_Multi::AJCharacter_Multi(const FObjectInitializer& ObjectIntializer)
: Super(ObjectIntializer)
, MoveForwardValue(0.0f)
, MoveRightValue(0.0f)
, bValidWidgetRenderTarget(false)
{
	bReplicates = true;
	bReplicateMovement = true;

	ReceiveDamageCollision = CreateDefaultSubobject<UJCapsuleReceiveDamageComponent>(TEXT("CapsuleReceiveDamageCollision"));
	ReceiveDamageCollision->SetupAttachment(RootComponent);

	VRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRRoot"));
	VRRoot->SetupAttachment(RootComponent);

	VRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VRCamera"));
	VRCamera->SetupAttachment(VRRoot);

	MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController"));
	MotionController->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	MotionController->bGenerateOverlapEvents = false;
	MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	MotionController->PlayerIndex = 0;
	MotionController->SetupAttachment(VRRoot);

	MotionControllerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MotionControllerMesh"));
	MotionControllerMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	MotionControllerMesh->bGenerateOverlapEvents = false;
	MotionControllerMesh->bOnlyOwnerSee = true;
	MotionControllerMesh->SetupAttachment(MotionController);

	GamePlayWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("GamePlayWidget"));
	GamePlayWidgetComp->SetupAttachment(VRCamera);

	GamePlayWidgetStereoLayer = CreateDefaultSubobject<UStereoLayerComponent>(TEXT("GamePlayWidgetStereoLayer"));
	GamePlayWidgetStereoLayer->bLiveTexture = true;
	GamePlayWidgetStereoLayer->bSupportsDepth = false;
	GamePlayWidgetStereoLayer->bNoAlphaChannel = false;
	GamePlayWidgetStereoLayer->SetupAttachment(VRCamera);
}

void AJCharacter_Multi::BeginPlay()
{
	Super::BeginPlay();
}

void AJCharacter_Multi::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	PlayerControllerPtr = nullptr;
	ProjectileManagerPtr = nullptr;
}

void AJCharacter_Multi::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (Role == ROLE_AutonomousProxy)
	{
		if (!bValidWidgetRenderTarget)
		{
			OnCheckValidWidgetRenderTarget();
		}
	}
}

void AJCharacter_Multi::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (UJGameInstance* GameInstance = Cast<UJGameInstance>(GetGameInstance()))
	{
		ProjectileManagerPtr = GameInstance->GetProjectileManager();
	}

	if (AJPlayerController_Multi* PlayerController = Cast<AJPlayerController_Multi>(NewController))
	{
		PlayerControllerPtr = PlayerController;
	}
}

void AJCharacter_Multi::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &AJCharacter_Multi::Fire);

	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AJCharacter_Multi::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AJCharacter_Multi::LookUp);
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AJCharacter_Multi::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AJCharacter_Multi::MoveRight);
}

void AJCharacter_Multi::OnCheckValidWidgetRenderTarget()
{
	if (UTextureRenderTarget2D* GamePlayWidgetRenderTarget = GamePlayWidgetComp->GetRenderTarget())
	{
		GamePlayWidgetStereoLayer->SetTexture(Cast<UTexture>(GamePlayWidgetRenderTarget));
		bValidWidgetRenderTarget = true;
	}
}

void AJCharacter_Multi::OnReceiveDamage(float Damage)
{
	Super::OnReceiveDamage(Damage);

	MulticastOnReceivedDamage(Damage);
}

void AJCharacter_Multi::OnDie()
{
	Super::OnDie();

	MulticastOnDie();
}
 
void AJCharacter_Multi::Fire()
{
	ServerFire(MotionControllerMesh->GetSocketTransform(GunMeshSocketName, RTS_World));
}

void AJCharacter_Multi::Turn(float Value)
{
	if (!bDie)
	{
		AddControllerYawInput(Value);
	}
}

void AJCharacter_Multi::LookUp(float Value)
{
	if (!bDie)
	{
		AddControllerPitchInput(Value);
	}
}

void AJCharacter_Multi::MoveForward(float Value)
{
	if (!bDie)
	{
		FRotator ControlYawRotation(0.0f, VRCamera->GetComponentRotation().Yaw, 0.f);
		float NewValue = Value * -1.f;
		ServerSetMoveForwardValue(NewValue);
		AddMovementInput(ControlYawRotation.Vector(), NewValue);
	}
}

void AJCharacter_Multi::MoveRight(float Value)
{
	if (!bDie)
	{
		FRotator ControlYawRotation(0.0f, VRCamera->GetComponentRotation().Yaw, 0.0f);
		ServerSetMoveRightValue(Value);
		AddMovementInput(ControlYawRotation.RotateVector(FVector::RightVector), Value);
	}
}

void AJCharacter_Multi::ServerFire_Implementation(const FTransform& ProjectileTransform)
{
	if (bDie) return;

	if (ProjectileManagerPtr.IsValid())
	{
		ProjectileManagerPtr->Fire(this, ProjectileTableKey, ProjectileTransform, 0.f, FRotator::ZeroRotator);
	}
}

void AJCharacter_Multi::ServerSetMoveForwardValue_Implementation(float InValue)
{
	MoveForwardValue = InValue;
}

void AJCharacter_Multi::ServerSetMoveRightValue_Implementation(float InValue)
{
	MoveRightValue = InValue;
}

void AJCharacter_Multi::MulticastOnReceivedDamage_Implementation(float Damage)
{
	ReceiveDamage(Damage);
}

void AJCharacter_Multi::MulticastOnDie_Implementation()
{
	Die();
	bDie = true;
}

void AJCharacter_Multi::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AJCharacter_Multi, MoveForwardValue, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(AJCharacter_Multi, MoveRightValue, COND_SimulatedOnly);
}
