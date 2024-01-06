// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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

	ASCharacter* PlayerCharacter;

	FTimerHandle TimerHandle_PowerupEffectTick;

	UPROPERTY(ReplicatedUsing = OnRep_PowerupCollected)
	bool bPowerupCollected;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	URotatingMovementComponent* RotatingComponent;

	UPROPERTY(BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* MeshComponent;


	//  ------------ Functions ------------  //

	//UFUNCTION()
	//void OnEffectTick();

	UFUNCTION()
	void OnRep_PowerupCollected();

	UFUNCTION()
	void OnPowerupStateChanged(bool bNewStateIsActive);

public:
	void AddPowerupToPlayer(AActor* PlayerToAddPowerup, TSubclassOf<ASPowerupObject> PowerupClass);

	void ActivatePowerup(AActor* PlayerToApplyPowerup);

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerup")
	void OnActivated(AActor* PlayerToApplyPowerup);

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerup")
	void OnExpired();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerup")
	void OnPowerupTicked();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
};
