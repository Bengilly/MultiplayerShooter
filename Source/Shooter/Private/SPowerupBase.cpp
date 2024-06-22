// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerupBase.h"
#include "Net/UnrealNetwork.h"
#include "SCharacter.h"

// Sets default values
ASPowerupBase::ASPowerupBase()
{
	TimeBetweenTicks = 0;
	TotalNumberOfTicks = 0;
	TickCount = 0;

	bReplicates = true;

	//bIsAbilityActive = false;
}

//only activate powerup once picked
void ASPowerupBase::ActivateAbility(AActor* PlayerToApplyAbilityTo)
{
	//UE_LOG(LogTemp, Log, TEXT("Ability Activated to: %s"), *FString(PlayerToApplyAbilityTo->GetName()));
	//UE_LOG(LogTemp, Log, TEXT("Powerup Activated: %s"), *FString(this->GetName()));

	AffectedActor = PlayerToApplyAbilityTo;

	OnAbilityActivated(AffectedActor);

	bIsAbilityActive = true;
	//OnRep_AbilityActive();

	if (TimeBetweenTicks > 0.0f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerupEffectTick, this, &ASPowerupBase::OnAbilityTick, TimeBetweenTicks, true);
	}
	else
	{
		OnAbilityTick();
	}
}

void ASPowerupBase::OnAbilityTick()
{
	TickCount++;
	GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Green, FString::Printf(TEXT("TickCount: %d"), TickCount));

	//effect finishes	
	if (TickCount >= TotalNumberOfTicks)
	{
		OnAbilityExpired();
		
		bIsAbilityActive = false;
		//OnRep_AbilityActive();

		TickCount = 0;

		//delete timer once the effect has finished
		GetWorldTimerManager().ClearTimer(TimerHandle_PowerupEffectTick);
	}
}

void ASPowerupBase::OnRep_AbilityActive()
{
	OnAbilityStateChanged(bIsAbilityActive);
}

void ASPowerupBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPowerupBase, bIsAbilityActive);	
}