#include "GameActor/Projectile/JBaseProjectile.h"

AJBaseProjectile::AJBaseProjectile(const FObjectInitializer& ObjectIntializer)
: Super(ObjectIntializer)
, bFire(false)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	bNetLoadOnClient = true;
	bReplicateMovement = true;
	bReplicates = true;
}

void AJBaseProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ProjectileParticleSystemComp->Deactivate();

	CollisionComponent->OnComponentHit.AddDynamic(this, &AJBaseProjectile::OnCollisionHit);
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AJBaseProjectile::OnCollisionBeginOverlap);
}

void AJBaseProjectile::BeginPlay()
{
	Super::BeginPlay();

	SetActorTickEnabled(false);
	DefaultActorRelativeRotation = RootComponent->RelativeRotation;
}

void AJBaseProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	CollisionComponent->OnComponentHit.RemoveDynamic(this, &AJBaseProjectile::OnCollisionHit);
	CollisionComponent->OnComponentBeginOverlap.RemoveDynamic(this, &AJBaseProjectile::OnCollisionBeginOverlap);
}

void AJBaseProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bFire)
	{
		return;
	}

	CurrLifeTime += DeltaSeconds;
	if (CurrLifeTime >= LifeTime)
	{
		Deactivate();
	}
}

void AJBaseProjectile::CreateSubobjects()
{
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 0.0f;
	ProjectileMovement->MaxSpeed = 0.0f;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	ProjectileMovement->bInitialVelocityInLocalSpace = false;
	ProjectileMovement->SetIsReplicated(true);
	ProjectileMovement->Velocity = FVector::ZeroVector;

	ProjectileParticleSystemComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ProjectileParticleSystem"));
	ProjectileParticleSystemComp->SetupAttachment(RootComponent);
}

void AJBaseProjectile::OnFire(const FJProjectileParameters& Parameters)
{
	LifeTime = Parameters.LifeTime;
	Damage = Parameters.Damage;
	ProjectileType = Parameters.ProjectileType;
	HitParticleSystem = Parameters.HitParticleSystem;

	FQuat FireRotation = Parameters.Transform.GetRotation();
	FVector StartLocation = Parameters.Transform.GetLocation() + (FireRotation.GetForwardVector() * Parameters.ForwardOffset);

	SetOwner(Parameters.Owner);
	AddCollisionIgnoreActor(Parameters.Owner);
	FRotator StartRotation = FireRotation.Rotator();
	StartRotation.Roll = 0.0f;
	StartRotation = StartRotation + DefaultActorRelativeRotation + Parameters.RotationOffset;
	SetActorRotation(StartRotation);
	SetActorLocation(StartLocation);
	SetCollisionSize(Parameters.CollisionSize);
	SetGravityScale(Parameters.GravityScale);
	SetMoveSpeed(Parameters.MoveSpeed);
	SetVelocity(FireRotation.GetForwardVector() * Parameters.MoveSpeed);
	Activate();
	MulticastActivate(Parameters.ProjectileParticleSystem, StartLocation, StartRotation);
}

void AJBaseProjectile::SetCollisionSize(const FVariant& CollisionSize)
{
}

void AJBaseProjectile::AddCollisionIgnoreActor(AActor* Actor)
{
	if (nullptr == Actor)
	{
		return;
	}

	CollisionComponent->MoveIgnoreActors.Add(Actor);
}

void AJBaseProjectile::RemoveCollisionIgnoreActor(AActor* Actor)
{
	if (nullptr == Actor)
	{
		return;
	}

	CollisionComponent->MoveIgnoreActors.Remove(Actor);
}

void AJBaseProjectile::SetGravityScale(float InGravityScale)
{
	ProjectileMovement->ProjectileGravityScale = InGravityScale;
}

void AJBaseProjectile::SetMoveSpeed(float InMoveSpeed)
{
	ProjectileMovement->InitialSpeed = InMoveSpeed;
	ProjectileMovement->MaxSpeed = InMoveSpeed;
}

void AJBaseProjectile::SetVelocity(FVector InVelocity)
{
	ProjectileMovement->Velocity = InVelocity;
}

void AJBaseProjectile::EnableCollision()
{
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->bGenerateOverlapEvents = true;
	CollisionComponent->Activate();
}

void AJBaseProjectile::DisableCollision()
{
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionComponent->bGenerateOverlapEvents = false;
	CollisionComponent->Deactivate();
}

void AJBaseProjectile::EnableProjectileMovement()
{
	ProjectileMovement->UpdatedComponent = CollisionComponent;
}

void AJBaseProjectile::DisableProjectileMovement()
{
	ProjectileMovement->UpdatedComponent = nullptr;
}

void AJBaseProjectile::Activate()
{
	CurrLifeTime = 0.0f;
	EnableCollision();
	EnableProjectileMovement();
	SetActorTickEnabled(true);
	bFire = true;
}

void AJBaseProjectile::Deactivate()
{
 	CurrLifeTime = 0.0f;
	RemoveCollisionIgnoreActor(GetOwner());
	SetOwner(nullptr);
	DisableCollision();
	DisableProjectileMovement();
 	SetActorTickEnabled(false);
	ReturnProjectileIndex();
	MulticastDeactivate();
	bFire = false;
}

void AJBaseProjectile::MulticastActivate_Implementation(UParticleSystem* InProjectileParticleSystem, const FVector& StartLocation, const FRotator& StartRotation)
{
	if (GetNetMode() == NM_Standalone)
	{
		SetProjectileParticleSystem(InProjectileParticleSystem);
	}
	else if (GetNetMode() == NM_Client)
	{
		DisableCollision();
		SetActorRotation(StartRotation);
		SetActorLocation(StartLocation);
		SetProjectileParticleSystem(InProjectileParticleSystem);
	}
}

void AJBaseProjectile::MulticastDeactivate_Implementation()
{
	if (GetNetMode() == NM_Standalone ||
		GetNetMode() == NM_Client)
	{
		ProjectileParticleSystemComp->Deactivate();
	}
}

void AJBaseProjectile::MulticastSpawnHitParticleSystem_Implementation(UParticleSystem* InHitParticleSystem, const FVector& ImpactPoint, const FVector& ImpactNormal)
{
	if (GetNetMode() == NM_Standalone ||
		GetNetMode() == NM_Client)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), InHitParticleSystem, ImpactPoint, ImpactNormal.Rotation());
	}
}

void AJBaseProjectile::SetProjectileParticleSystem(UParticleSystem* InParticleSystem)
{
	ProjectileParticleSystemComp->Activate();
	ProjectileParticleSystemComp->SetTemplate(InParticleSystem);
}

void AJBaseProjectile::ReturnProjectileIndex()
{
	if (UJGameInstance* GameInstance = Cast<UJGameInstance>(GetGameInstance()))
	{
		if (UJProjectileManager* ProjectileManager = GameInstance->GetProjectileManager())
		{
			ProjectileManager->EnqueueProjectileIndex(ProjectileType, ProjectileIndex);
		}
	}
}

void AJBaseProjectile::OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpuse, const FHitResult& Hit)
{
	if (AActor* OtherOwner = OtherComp->GetOwner())
	{
		if (GetOwner() == OtherOwner)
		{
			return;
		}
	}

	MulticastSpawnHitParticleSystem(HitParticleSystem, Hit.ImpactPoint, Hit.ImpactNormal);

	if (HasAuthority())
	{
		Deactivate();
	}
}

void AJBaseProjectile::OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AActor* OtherOwner = OtherComp->GetOwner())
	{
		if (GetOwner() == OtherOwner)
		{
			return;
		}
	}

	MulticastSpawnHitParticleSystem(HitParticleSystem, SweepResult.ImpactPoint, SweepResult.ImpactNormal);

	if (HasAuthority())
	{
		Deactivate();
	}
}
