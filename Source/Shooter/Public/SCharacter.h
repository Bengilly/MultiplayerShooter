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

UCLASS()
class SHOOTER_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:

	//  ------------ Variables ------------  //

	FTimerHandle Timerhandle_Reload;
	bool bIsShooting;
	float DefaultFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	USoundBase* StartReloadSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	USoundBase* EndReloadSound;

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	int PlayerRifleAmmo;

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	int PlayerPistolAmmo;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
	bool bIsReloading;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
	bool bIsZooming;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
	bool bSprinting;
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	ASWeapon* CurrentWeapon;

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


	//  ------------ Functions ------------  //


	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

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
	void ReloadWeapon();
	void EquipRifle();
	void EquipPistol();

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* CharacterHealthComp, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

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
