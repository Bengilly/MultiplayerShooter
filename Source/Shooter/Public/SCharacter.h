 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ASWeapon;
class USHealthComponent;
class UAnimMontage;
class USoundBase;
class USkeletalMeshComponent;

USTRUCT(BlueprintType)
struct FWeaponInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	ASWeapon* Weapon;

	UPROPERTY(BlueprintReadOnly)
	int Ammo;
};

UCLASS()
class SHOOTER_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:

	//  ------------ Variables ------------  //
	float DefaultFOV;
	FTimerHandle Timerhandle_Reload;
	FTimerHandle TimerHandler_SwitchWeapon;

	UPROPERTY(Replicated, BlueprintReadOnly)
	FWeaponInfo WeaponInfoStruct;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Weapon")
	TArray<FWeaponInfo> WeaponStructArray;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponSwitch, BlueprintReadOnly)
	ASWeapon* CurrentWeapon;

	UPROPERTY(ReplicatedUsing = OnRep_StartWeaponSwitch)
	bool bIsSwitchingWeapon;

	UPROPERTY(Replicated)
	TArray<ASWeapon*> WeaponClassArray;

	//map to store player ammo for corresponding weapon
	//UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	//TMap<ASWeapon*, int> WeaponAmmoMap;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	UAnimMontage* ReloadAnim;

	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Animations")
	UAnimMontage* SwitchWeaponAnim;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Player")
	int PlayerRifleAmmo;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Player")
	int PlayerPistolAmmo;

	UPROPERTY(ReplicatedUsing = OnRep_Reloading, BlueprintReadOnly, Category = "Player")
	bool bIsReloading;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
	bool bIsZooming;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
	bool bSprinting;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player")
	float MaxStamina;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float StaminaUsageRate;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float StaminaRegenRate;

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	float CurrentStamina;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
	bool bIsShooting;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
	bool bPlayerDied;

	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
	FName RifleAttachSocketName;

	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
	FName PistolAttachSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<ASWeapon> RifleWeaponClass;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<ASWeapon> PistolWeaponClass;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float ZoomedFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1, ClampMax = 100));
	float ZoomInterpSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USHealthComponent* HealthComponent;

	//  ------------ Audio ------------  // 

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	USoundBase* StartReloadSound;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	USoundBase* EndReloadSound;

	//  ------------ Functions ------------  //


	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void InitialiseDefaultWeapons(TSubclassOf<ASWeapon> WeaponClass, int AmmoValue, FName SocketName);
	void AddWeapon(ASWeapon* Weapon);
	void SetCurrentWeapon(ASWeapon* NewWeapon, ASWeapon* PreviousWeapon);
	bool CanSwitchWeapon(ASWeapon* Weapon);

	void MoveForward(float value);
	void MoveRight(float value);
	void BeginCrouch();
	void EndCrouch();
	void ZoomIn();
	void ZoomOut();
	void StartShooting();
	void StopShooting();
	void StartSprinting();
	void StopSprinting();
	void StartReload();
	void StartJumping();



	UFUNCTION()
	void ReloadWeapon(ASWeapon* EquippedWeapon);

	UFUNCTION()
	void EquipWeapon(ASWeapon* Weapon);
	void SwitchToRifle();
	void SwitchToPistol();

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* CharacterHealthComp, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	//  ------------ Multiplayer Functions ------------  //

	UFUNCTION()
	void OnRep_Reloading();

	UFUNCTION()
	void OnRep_StartWeaponSwitch();
	
	UFUNCTION(Server, Reliable)
	void ServerStartReload();
	
	UFUNCTION(Server, Reliable)
	void ServerReloadWeapon(ASWeapon* EquippedWeapon);

	UFUNCTION()
	void OnRep_WeaponSwitch(ASWeapon* PreviousWeapon);

	UFUNCTION (Server, Reliable)
	void ServerEquipWeapon(ASWeapon* Weapon);
	
	UFUNCTION(Server, Reliable)
	void ServerSwitchToRifle();

	UFUNCTION(Server, Reliable)
	void ServerSwitchToPistol();
	
	UFUNCTION(Server, Reliable)
	void ServerZoomIn();

	UFUNCTION(Server, Reliable)
	void ServerZoomOut();

	UFUNCTION(Server, Reliable)
	void ServerStartSprinting();

	UFUNCTION(Server, Reliable)
	void ServerStopSprinting();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;

};
