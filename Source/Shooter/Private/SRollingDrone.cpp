// Fill out your copyright notice in the Description page of Project Settings.


#include "SRollingDrone.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "DrawDebugHelpers.h"
#include "Components/SHealthComponent.h"
#include "Components/SphereComponent.h"
#include "SCharacter.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"


// Sets default values
ASRollingDrone::ASRollingDrone()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//mesh comp
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;
	MeshComponent->SetCanEverAffectNavigation(false);
	MeshComponent->SetSimulatePhysics(true);

	//health comp
	HealthComponent = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->OnHealthChanged.AddDynamic(this, &ASRollingDrone::HandleTakeDamage);

	//audio comp
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(MeshComponent);

	//sphere comp
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetSphereRadius(300);
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	RequiredDistanceToTargetLocation = 500;
	bAccelerationChange = true;
	MovementForce = 1000;
	ExplosionDamage = 50;
	ExplosionRadius = 300;

}

// Called when the game starts or when spawned
void ASRollingDrone::BeginPlay()
{
	Super::BeginPlay();

	//find first point on path

	if (GetLocalRole() == ROLE_Authority)
	{
		NextPointLocation = GetNextPoint();
	}

	AudioComponent = UGameplayStatics::SpawnSoundAttached(ChaseSound, RootComponent);
}

// Called every frame
void ASRollingDrone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float MovementVelocity = GetVelocity().Size();
	float VelocityVolume = FMath::GetMappedRangeValueClamped(FVector2D(10, 1000), FVector2D(0.1, 2), MovementVelocity);
	AudioComponent->SetVolumeMultiplier(VelocityVolume);

	if (GetLocalRole() == ROLE_Authority && !bHasExploded)
	{
		float DistanceToTargetLocation = (GetActorLocation() - NextPointLocation).Size();

		//get next path point if close to the player, keep adding force if not
		if (DistanceToTargetLocation <= RequiredDistanceToTargetLocation)
		{
			NextPointLocation = GetNextPoint();
			DrawDebugString(GetWorld(), GetActorLocation(), "Target Reached");
		}
		else
		{
			FVector ForceDirection = NextPointLocation - GetActorLocation();
			ForceDirection.Normalize();
			ForceDirection *= MovementForce;

			MeshComponent->AddForce(ForceDirection, NAME_None, bAccelerationChange);

			DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + ForceDirection, 32, FColor::Yellow, false, 0.0f, 0, 1.0f);
		}

		DrawDebugSphere(GetWorld(), NextPointLocation, 20, 12, FColor::Yellow, false, 4.0f, 1.0f);
	}

}


FVector ASRollingDrone::GetNextPoint()
{
	ACharacter* PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);
	UNavigationPath* NavigationPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);

	if (NavigationPath == nullptr)
	{

	}
	//path[0] = actor location - so I want to get the next path value in the array
	else if (NavigationPath->PathPoints.Num() > 1)
	{
		return NavigationPath->PathPoints[1];
	}

	//can't find path
	return GetActorLocation();
}

void ASRollingDrone::HandleTakeDamage(USHealthComponent* DroneHealthComp, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (DroneMaterialInstance == nullptr)
	{
		DroneMaterialInstance = MeshComponent->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComponent->GetMaterial(0));
	}

	if (DroneMaterialInstance)
	{
		DroneMaterialInstance->SetScalarParameterValue("DamageTaken", GetWorld()->TimeSeconds);
	}

	UE_LOG(LogTemp, Log, TEXT("Health %s of %s"), *FString::SanitizeFloat(Health), *GetName());

	// explode once health reaches 0
	if (Health <= 0.0f)
	{
		Explode();
	}
}

void ASRollingDrone::Explode()
{
	if (bHasExploded)
	{
		return;
	}

	bHasExploded = true;
	
	UGameplayStatics::PlaySoundAtLocation(this, ExplodeSound, GetActorLocation());
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	MeshComponent->SetVisibility(false, true);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AudioComponent->Stop();

	//destroy on server, propogated onto client due to drone object being replicated
	if (GetLocalRole() == ROLE_Authority)
	{
		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(this);
		UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);

		DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12, FColor::Red, false, 4.0f, 1.0f);

		//delete object after 2sec
		SetLifeSpan(2.0f);
	}
}

void ASRollingDrone::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	ASCharacter* PlayerPawn = Cast<ASCharacter>(OtherActor);

	if (PlayerPawn && !bHasExploded)
	{
		Explode();
	}
}
