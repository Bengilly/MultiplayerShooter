// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SEnumAbilityPickup.h"
#include "SPowerupObject.generated.h"

class URotatingMovementComponent;
class UStaticMeshComponent;
class ASCharacter;

UCLASS()
class SHOOTER_API ASPowerupObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPowerupObject();

protected:

	//  ------------ Variables ------------  //

	UPROPERTY(BlueprintReadWrite)
	EAbilityPickupType AbilityPickupType;

	ASCharacter* PlayerCharacter;

	FTimerHandle TimerHandle_PowerupEffectTick;

	UPROPERTY(ReplicatedUsing = OnRep_AbilityCollected)
	bool bAbilityCollected;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	URotatingMovementComponent* RotatingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* MeshComponent;


	//  ------------ Functions ------------  //

	//UFUNCTION()
	//void OnEffectTick();

	UFUNCTION()
	void OnRep_AbilityCollected();

	UFUNCTION()
	void OnAbilityPickupStateChanged(bool bNewStateIsActive);

public:

	void AddPowerupToPlayer(AActor* PlayerToAddPowerup);

	void ActivatePowerup(AActor* PlayerToApplyPowerup);

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerup")
	void OnActivated(AActor* PlayerToApplyPowerup);

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerup")
	void OnExpired();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerup")
	void OnPowerupTicked();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
};
