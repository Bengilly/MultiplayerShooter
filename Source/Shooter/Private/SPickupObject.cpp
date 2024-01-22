// Fill out your copyright notice in the Description page of Project Settings.


#include "SPickupObject.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "TimerManager.h"
#include "SPowerupObject.h"
#include "GameFramework/RotatingMovementComponent.h"

// Sets default values
ASPickupObject::ASPickupObject()
{
	SpawnCooldown = 5.0f;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetSphereRadius(60.0f);
	RootComponent = SphereComponent;

	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
	DecalComponent->SetRelativeRotation(FRotator(90, 0.0f, 0.0f));
	DecalComponent->DecalSize = FVector(64, 60, 60);
	DecalComponent->SetupAttachment(RootComponent);

	bReplicates = true;
}

// Called when the game starts or when spawned
void ASPickupObject::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		SpawnPickup();
	}
}

void ASPickupObject::NotifyActorBeginOverlap(AActor* Player)
{
	Super::NotifyActorBeginOverlap(Player);

	//add powerup to player if it has been successfully spawned
	if (AbilityPickupInstance && GetLocalRole() == ROLE_Authority)
	{
		UE_LOG(LogTemp, Log, TEXT("AbilityPickupClass: %s"), *FString(AbilityPickupClass->GetName()));
		AbilityPickupInstance->AddPowerupToPlayer(Player);
		AbilityPickupInstance = nullptr;

		GetWorldTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &ASPickupObject::SpawnPickup, SpawnCooldown);
	}
}

void ASPickupObject::SpawnPickup()
{
	//check if Powerup Class is setup correctly and don't spawn object if null
	if (AbilityPickupClass == nullptr)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AbilityPickupInstance = GetWorld()->SpawnActor<ASPowerupObject>(AbilityPickupClass, GetTransform(), SpawnParams);
}
