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
		//UE_LOG(LogTemp, Log, TEXT("AbilityPickup: %s"), *FString(AbilityPickupType->GetName()));
		AbilityPickupInstance->AddPowerupToPlayer(Player);
		AbilityPickupInstance = nullptr;

		GetWorldTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &ASPickupObject::SpawnPickup, SpawnCooldown);
	}
}

void ASPickupObject::SpawnPickup()
{
	//check if powerup classes are setup correctly and don't spawn object if null
	for (int32 i = 0; i < AbilityPickupType.Num(); i++)
	{
		if (AbilityPickupType[i] == nullptr)
		{
			return;
		}
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	float RandomSpawnValue = FMath::FRand();
	if (RandomSpawnValue < 0.5)
	{
		AbilityPickupInstance = GetWorld()->SpawnActor<ASPowerupObject>(AbilityPickupType[0], GetTransform(), SpawnParams);
	}
	else if (RandomSpawnValue > 0.5)
	{
		AbilityPickupInstance = GetWorld()->SpawnActor<ASPowerupObject>(AbilityPickupType[1], GetTransform(), SpawnParams);
	}




}