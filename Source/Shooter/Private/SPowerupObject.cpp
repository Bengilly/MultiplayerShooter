// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerupObject.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "SCharacter.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASPowerupObject::ASPowerupObject()
{
	bReplicates = true;

	bAbilityCollected = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = MeshComponent;

	//spin powerup object around the Z axis
	RotatingComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingComponent"));
	RotatingComponent->RotationRate.Yaw = 180.0f;
}


void ASPowerupObject::AddPowerupToPlayer(AActor* Player)
{
	int ChargesToAdd = 1;
	PlayerCharacter = Cast<ASCharacter>(Player);
	PlayerCharacter->AddPowerupChargeToPlayer(AbilityPickupType, ChargesToAdd);

	bAbilityCollected = true;
	OnRep_AbilityCollected();
	Destroy();
}

//void ASPowerupObject::AddPowerupToPlayer(AActor* Player, TSubclassOf<ASPowerupObject> PowerupClass)
//{
//	int ChargesToAdd = 1;
//	
//	PlayerCharacter = Cast<ASCharacter>(Player);
//	PlayerCharacter->AddPowerupChargeToPlayer(PowerupClass, ChargesToAdd);
//
//	bAbilityCollected = true;
//	OnRep_AbilityCollected();
//	Destroy();
//}

void ASPowerupObject::OnAbilityPickupStateChanged(bool bNewStateIsActive)
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

void ASPowerupObject::OnRep_AbilityCollected()
{
	OnAbilityPickupStateChanged(bAbilityCollected);
}

void ASPowerupObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPowerupObject, bAbilityCollected);
}

