// Fill out your copyright notice in the Description page of Project Settings.


#include "SPickupObject.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "TimerManager.h"
#include "SPowerupObject.h"

// Sets default values
ASPickupObject::ASPickupObject()
{
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetSphereRadius(60.0f);
	RootComponent = SphereComponent;

	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
	DecalComponent->SetRelativeRotation(FRotator(90, 0.0f, 0.0f));
	DecalComponent->DecalSize = FVector(64, 60, 60);
	DecalComponent->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ASPickupObject::BeginPlay()
{
	Super::BeginPlay();

	SpawnPickup();
	
}

void ASPickupObject::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	//activate powerup if it has been successfully spawned
	if (PowerupInstance)
	{
		PowerupInstance->ActivatePowerupObject();
		PowerupInstance = nullptr;

		GetWorldTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &ASPickupObject::SpawnPickup, SpawnCooldown);
	}

}

void ASPickupObject::SpawnPickup()
{
	//check if Powerup Class is setup correctly and don't spawn object if null
	if (PowerupClass == nullptr)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	PowerupInstance = GetWorld()->SpawnActor<ASPowerupObject>(PowerupClass, GetTransform(), SpawnParams);
}
