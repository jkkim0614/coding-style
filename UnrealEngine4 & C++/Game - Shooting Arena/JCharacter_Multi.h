#pragma once

#include "GunFighter.h"
#include "GameActor/Pawn/JCharacterBase.h"
#include "JCharacter_Multi.generated.h"

UCLASS(BlueprintType)
class AJCharacter_Multi : public AJCharacterBase
{
	GENERATED_BODY()

public:
	AJCharacter_Multi(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void PossessedBy(AController* NewController) override;

	UFUNCTION(BlueprintCallable, Category=Character_Multi)
	float GetMoveForwardValue() const	{ return MoveForwardValue; }

	UFUNCTION(BlueprintCallable, Category=Character_Multi)
	float GetMoveRightValue() const		{ return MoveRightValue; }

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire(const FTransform& ProjectileTransform);

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void OnReceiveDamage(float Damage) override;
	virtual void OnDie() override;

private:
	void Initialize();

	void Fire();
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);

	void OnCheckValidWidgetRenderTarget();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetMoveForwardValue(float InValue);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetMoveRightValue(float InValue);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnReceivedDamage(float Damage);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnDie();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Character_Multi)
	class USceneComponent* VRRoot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Character_Multi)
	class UCameraComponent* VRCamera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Character_Multi)
	class UMotionControllerComponent* MotionController;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Character_Multi)
	class UStaticMeshComponent* MotionControllerMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Character_Multi)
	class UWidgetComponent* GamePlayWidgetComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Character_Multi)
	class UStereoLayerComponent* GamePlayWidgetStereoLayer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Character_Multi)
	class UJCapsuleReceiveDamageComponent* ReceiveDamageCollision;

private:
	UPROPERTY(EditDefaultsOnly, Category="Character_Multi | Projectile")
	FName ProjectileTableKey;

	UPROPERTY(EditDefaultsOnly, Category="Character_Multi | Projectile")
	FName GunMeshSocketName;

private:
	UPROPERTY(Replicated)
	float MoveForwardValue;

	UPROPERTY(Replicated)
	float MoveRightValue;

	TWeakObjectPtr<class UJProjectileManager> ProjectileManagerPtr;
	TWeakObjectPtr<class AJPlayerController_Multi> PlayerControllerPtr;

	bool bValidWidgetRenderTarget;
};
