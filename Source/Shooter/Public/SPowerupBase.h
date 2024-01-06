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

	void ActivateAbility(AActor* PlayerToApplyPowerup);

protected:

	UPROPERTY(ReplicatedUsing=OnRep_AbilityActive)
	bool bAbilityActive;

	//current tick count
	int32 TickCount;

	UPROPERTY(EditDefaultsOnly, Category = "Powerup")
	float TimeBetweenTicks;

	//total number of ticks that the powerup lasts for
	UPROPERTY(EditDefaultsOnly, Category = "Powerup")
	int32 TotalNumberOfTicks;

	FTimerHandle TimerHandle_PowerupEffectTick;

	void OnEffectTick();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerup")
	void OnActivated(AActor* PlayerToApplyPowerup);

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerup")
	void OnExpired();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerup")
	void OnAbilityStateChanged(bool bIsInvisible);

	UFUNCTION()
	void OnRep_AbilityActive();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UFUNCTION(Server, Reliable)
	void ServerActivateAbility(AActor* PlayerToApplyPowerup);

};
