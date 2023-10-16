// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;
class USoundBase;

//multiplayer -- holds data for single weapon trace
USTRUCT()
struct FHitscanLineTrace 
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> SurfaceType;

	UPROPERTY()
	FVector_NetQuantize TraceEnd;
};

UCLASS()
class SHOOTER_API ASWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeapon();

	virtual void ShootWeapon();

	UFUNCTION(Server, Reliable, WithValidation)
	virtual void ServerShootWeapon();

	void StartShooting();
	void StopShooting();
	void PlayImpactEffects(EPhysicalSurface SurfaceType, FVector TraceImpactLocation);
	USkeletalMeshComponent* GetWeaponMesh();

	int GetCurrentAmmo();
	int QueryAmmoMissing();
	void Reload(int BulletsToAdd);
	bool CheckIfMagazineFull();

	void PlayEquipAudio();
	void PlayUnEquipAudio();

protected:

	//  ------------ Variables ------------  //

	FTimerHandle TimerHandler_TimeBetweenShots;
	float TimeBetweenShots;
	float TimeSinceLastShot;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	int MaxAmmoPerMagazine;

	//horizontal bullet spread (degrees)
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (ClampMin = 0.0f))
	float HorizontalBulletSpread;

	//vertical bullet spread (degrees)
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (ClampMin = 0.0f))
	float VerticalBulletSpread;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	int CurrentAmmo;

	UPROPERTY(ReplicatedUsing=OnRep_LineTrace)
	FHitscanLineTrace LineTrace;

	//bullets shot per minute
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float RateOfFire;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float BodyDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float HeadshotDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UCameraShakeBase> CameraShake;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* MuzzleFlashEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* ImpactEffectBodyshot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* ImpactEffectHeadshot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* ImpactEffectDefault;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* WeaponTracerEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	USoundBase* ShootSound;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName MuzzleSocketName;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName TracerTargetName;

	//  ------------ Audio ------------  // 

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	USoundBase* EquipSound;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	USoundBase* UnEquipSound;


	//  ------------ Functions ------------  //

	virtual void BeginPlay() override;

	void PlayWeaponEffects(FVector TracerEndPoint);

	UFUNCTION()
	void OnRep_LineTrace();

public:	


};
