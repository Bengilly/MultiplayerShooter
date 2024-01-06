// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerupBase.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASPowerupBase::ASPowerupBase()
{
	TimeBetweenTicks = 0;
	TotalNumberOfTicks = 0;
	TickCount = 0;

	bReplicates = true;

	bAbilityActive = false;
}

void ASPowerupBase::OnEffectTick()
{
	TickCount++;

	//OnPowerupTicked();

	//effect finishes	
	if (TickCount >= TotalNumberOfTicks)
	{
		OnExpired();

		bAbilityActive = false;
		UE_LOG(LogTemp, Log, TEXT("Powerup Deactivated"));

		OnRep_AbilityActive();

		//delete timer once the effect has finished
		GetWorldTimerManager().ClearTimer(TimerHandle_PowerupEffectTick);
	}
}

//only activate powerup once picked
void ASPowerupBase::ActivateAbility(AActor* PlayerToApplyPowerup)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		UE_LOG(LogTemp, Log, TEXT("Powerup Activated"));

		OnActivated(PlayerToApplyPowerup);

		bAbilityActive = true;
		OnRep_AbilityActive();

		if (TimeBetweenTicks > 0.0f)
		{
			GetWorldTimerManager().SetTimer(TimerHandle_PowerupEffectTick, this, &ASPowerupBase::OnEffectTick, TimeBetweenTicks, true);
		}
		else
		{
			OnEffectTick();
		}
	}
	else
	{
		ServerActivateAbility(PlayerToApplyPowerup);
	}
}

void ASPowerupBase::OnRep_AbilityActive()
{
	OnAbilityStateChanged(bAbilityActive);
}

void ASPowerupBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPowerupBase, bAbilityActive);
}

void ASPowerupBase::ServerActivateAbility_Implementation(AActor* PlayerToApplyPowerup)
{
	ActivateAbility(PlayerToApplyPowerup);
}
