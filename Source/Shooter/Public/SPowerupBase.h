// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPowerupBase.generated.h"

UCLASS()
class SHOOTER_API ASPowerupBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPowerupBase();

	void ActivateAbility(AActor* PlayerToApplyAbilityTo);

protected:

	UPROPERTY(ReplicatedUsing=OnRep_AbilityActive)
	bool bIsAbilityActive;

	//current tick count
	int32 TickCount;

	UPROPERTY(EditDefaultsOnly, Category = "Powerup")
	float TimeBetweenTicks;

	//total number of ticks that the powerup lasts for
	UPROPERTY(EditDefaultsOnly, Category = "Powerup")
	int32 TotalNumberOfTicks;

	FTimerHandle TimerHandle_PowerupEffectTick;

	void OnAbilityTick();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerup")
	void OnAbilityActivated(AActor* PlayerToApplyPowerup);

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerup")
	void OnAbilityExpired();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerup")
	void OnAbilityStateChanged(bool bIsInvisible);

	UFUNCTION()
	void OnRep_AbilityActive();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

};
