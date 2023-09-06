// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPowerupObject.generated.h"

class URotatingMovementComponent;
class UStaticMeshComponent;

UCLASS()
class SHOOTER_API ASPowerupObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPowerupObject();

protected:

	//  ------------ Variables ------------  //

	int32 TickCount;

	FTimerHandle TimerHandle_PowerupEffectTick;

	UPROPERTY(ReplicatedUsing=OnRep_PowerupActive)
	bool bPowerupActive;
	
	UPROPERTY(EditDefaultsOnly, Category = "Powerup")
	float TimeBetweenTicks;

	//total number of ticks that the powerup lasts for
	UPROPERTY(EditDefaultsOnly, Category = "Powerup")
	int32 TotalNumberOfTicks;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	URotatingMovementComponent* RotatingComponent;

	UPROPERTY(BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* MeshComponent;


	//  ------------ Functions ------------  //

	UFUNCTION()
	void OnEffectTick();

	UFUNCTION()
	void OnRep_PowerupActive();

	UFUNCTION()
	void OnPowerupStateChanged(bool bNewStateIsActive);

public:

	void ActivatePowerupObject(AActor* PlayerToApplyPowerup);

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerup")
	void OnActivated(AActor* PlayerToApplyPowerup);

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerup")
	void OnExpired();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerup")
	void OnPowerupTicked();
};
