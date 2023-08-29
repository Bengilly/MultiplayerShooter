// Fill out your copyright notice in the Description page of Project Settings.


#include "SExplosive.h"
#include "Components/SHealthComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASExplosive::ASExplosive()
{
	HealthComponent = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->OnHealthChanged.AddDynamic(this, &ASExplosive::OnHealthChanged);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetSimulatePhysics(true);
	MeshComponent->SetCollisionObjectType(ECC_PhysicsBody);		//set physics body so nearby explosions effect this object
	RootComponent = MeshComponent;

	RadialForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComponent"));
	RadialForceComponent->SetupAttachment(MeshComponent);
	RadialForceComponent->Radius = 400;
	RadialForceComponent->bImpulseVelChange = true;
	RadialForceComponent->bAutoActivate = false;
	RadialForceComponent->bIgnoreOwningActor = true;

	ExplosionPower = 500;

	SetReplicates(true);
	SetReplicateMovement(true);
}


void ASExplosive::OnHealthChanged(USHealthComponent* ExplosiveHealthComp, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{

	if (bExploded)
	{
		return;
	}
	else if (Health <= 0.0f)
	{
		bExploded = true;

		//call on server and client after exploded value is set to true
		OnRep_Exploded();

		// fire object upwards
		FVector BoostPower = FVector::UpVector * ExplosionPower;
		MeshComponent->AddImpulse(BoostPower, NAME_None, true);

		RadialForceComponent->FireImpulse();
	}
}

//  ------------ Multiplayer Functions ------------  //

void ASExplosive::OnRep_Exploded()
{
	//play effects and change object material
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionFX, GetActorLocation());
	MeshComponent->SetMaterial(0, ExplodedMaterial);
}

void ASExplosive::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//replicate variables
	DOREPLIFETIME(ASExplosive, bExploded);
}