#pragma once

#include "GunFighter.h"
#include "JGlobalDefine.h"
#include "Components/ShapeComponent.h"
#include "JBaseProjectile.generated.h"

UCLASS()
class AJBaseProjectile : public AActor
{
	GENERATED_BODY()

public:
	AJBaseProjectile(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;

	void SetProjectileIndex(uint8 InProjectileIndex)	{ ProjectileIndex = InProjectileIndex; }
	uint8 GetProjectileIndex() const					{ return ProjectileIndex; }

	virtual void OnFire(const FJProjectileParameters& Parameters);

protected:
	template<typename T>
	void CreateCollisionComponent()
	{
		CollisionComponent = CreateDefaultSubobject<T>(TEXT("Collision"));
		CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
	}

	void CreateSubobjects();

	virtual void SetCollisionSize(const FVariant& CollisionSize);

	UFUNCTION()
	virtual void OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpuse, const FHitResult& Hit);

	UFUNCTION()
	virtual void OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastActivate(UParticleSystem* InProjectileParticleSystem, const FVector& StartLocation, const FRotator& StartRotation);

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastDeactivate();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpawnHitParticleSystem(UParticleSystem* InHitParticleSystem, const FVector& ImpactPoint, const FVector& ImpactNormal);

private:
	void AddCollisionIgnoreActor(AActor* Actor);
	void RemoveCollisionIgnoreActor(AActor* Actor);
	void SetGravityScale(float InGravityScale);
	void SetMoveSpeed(float InMoveSpeed);
	void SetVelocity(FVector InVelocity);
	void SetProjectileParticleSystem(UParticleSystem* InParticleSystem);
	void EnableProjectileMovement();
	void EnableCollision();
	void DisableProjectileMovement();
	void DisableCollision();
	void Activate();
	void Deactivate();
	void ReturnProjectileIndex();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=BaseProjectile)
	class UShapeComponent* CollisionComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=BaseProjectile)
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=BaseProjectile)
	class UParticleSystemComponent* ProjectileParticleSystemComp;

private:
	UPROPERTY(Transient)
	UParticleSystem* HitParticleSystem;

	FRotator DefaultActorRelativeRotation;
	EJProjectileType ProjectileType;
	float LifeTime;
	float CurrLifeTime;
	float Damage;
	uint8 ProjectileIndex;
	bool bFire;
};
