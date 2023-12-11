// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerupObject.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASPowerupObject::ASPowerupObject()
{
	TimeBetweenTicks = 0;
	TotalNumberOfTicks = 0;
	TickCount = 0;
	//SetReplicates(true);
	bReplicates = true;
	bPowerupActive = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = MeshComponent;

	//spin powerup object around the Z axis
	RotatingComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingComponent"));
	RotatingComponent->RotationRate.Yaw = 180.0f;
}

void ASPowerupObject::OnEffectTick()
{
	TickCount++;

	OnPowerupTicked();

	//effect finishes	
	if (TickCount >= TotalNumberOfTicks)
	{
		OnExpired();
		bPowerupActive = false;
		OnRep_PowerupActive();

		//delete timer once the effect has finished
		GetWorldTimerManager().ClearTimer(TimerHandle_PowerupEffectTick);
	}
}

//only activate powerup once picked
void ASPowerupObject::ActivatePowerupObject(AActor* PlayerToApplyPowerup)
{
	OnActivated(PlayerToApplyPowerup);

	//call onreplication on the server as this was only being called by the clients
	bPowerupActive = true;
	OnRep_PowerupActive();

	if (TimeBetweenTicks > 0.0f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerupEffectTick, this, &ASPowerupObject::OnEffectTick, TimeBetweenTicks, true);
	}
	else
	{
		OnEffectTick();
	}
}

void ASPowerupObject::OnPowerupStateChanged(bool bNewStateIsActive)
{
	if (bNewStateIsActive)
	{
		MeshComponent->SetVisibility(false, true);
	}
	else
	{
		MeshComponent->SetVisibility(true, true);
	}
}

void ASPowerupObject::OnRep_PowerupActive()
{
	OnPowerupStateChanged(bPowerupActive);
}

void ASPowerupObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPowerupObject, bPowerupActive);
}