// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerupObject.h"

// Sets default values
ASPowerupObject::ASPowerupObject()
{
	TimeBetweenTicks = 0;
	TotalNumberOfTicks = 0;
	TickCount = 0;
}

// Called when the game starts or when spawned
void ASPowerupObject::BeginPlay()
{
	Super::BeginPlay();
}

void ASPowerupObject::OnEffectTick()
{
	TickCount++;

	OnPowerupTicked();

	//effect finishes
	if (TickCount >= TotalNumberOfTicks)
	{
		OnExpired();

		//delete timer once the effect has finished
		GetWorldTimerManager().ClearTimer(TimerHandle_PowerupEffectTick);
	}
}

//only activate powerup once picked up
void ASPowerupObject::ActivatePowerupObject()
{
	if (TimeBetweenTicks > 0.0f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerupEffectTick, this, &ASPowerupObject::OnEffectTick, TimeBetweenTicks, true, 0.0f);
	}
	else
	{
		OnEffectTick();
	}
}