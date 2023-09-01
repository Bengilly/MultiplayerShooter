// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPowerupObject.generated.h"

UCLASS()
class SHOOTER_API ASPowerupObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPowerupObject();

protected:

	//  ------------ Variables ------------  //

	UPROPERTY(EditDefaultsOnly, Category = "Powerup")
	float TimeBetweenTicks;

	//total number of ticks that the powerup lasts for
	UPROPERTY(EditDefaultsOnly, Category = "Powerup")
	int32 TotalNumberOfTicks;

	int32 TickCount;

	FTimerHandle TimerHandle_PowerupEffectTick;


	//  ------------ Functions ------------  //

	UFUNCTION()
	void OnEffectTick();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:

	void ActivatePowerupObject();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerup")
	void OnActivated();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerup")
	void OnExpired();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerup")
	void OnPowerupTicked();
};
