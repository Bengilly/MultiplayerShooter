// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SRollingDrone.generated.h"

class UStaticMeshComponent;
class USHealthComponent;
class USphereComponent;
class USoundCue;
class UAudioComponent;

UCLASS()
class SHOOTER_API ASRollingDrone : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASRollingDrone();

protected:

	//  ------------ Variables ------------  //

	FVector NextPointLocation;
	float bHasExploded;

	UPROPERTY(EditDefaultsOnly, Category = "Rolling Drone")
	float RequiredDistanceToTargetLocation;

	UPROPERTY(EditDefaultsOnly, Category = "Rolling Drone")
	bool bAccelerationChange;

	UPROPERTY(EditDefaultsOnly, Category = "Rolling Drone")
	float MovementForce;

	UPROPERTY(EditDefaultsOnly, Category = "Rolling Drone")
	float ExplosionDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Rolling Drone")
	float ExplosionRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Rolling Drone")
	UParticleSystem* ExplosionEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Rolling Drone")
	USoundCue* ExplodeSound;

	UPROPERTY(EditDefaultsOnly, Category = "Rolling Drone")
	USoundCue* ChaseSound;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USHealthComponent* HealthComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USphereComponent* SphereComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UAudioComponent* AudioComponent;

	//pulse material on take damage
	UMaterialInstanceDynamic* DroneMaterialInstance;

	//  ------------ Functions ------------  //

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FVector GetNextPoint();

	UFUNCTION()
	void HandleTakeDamage(USHealthComponent* DroneHealthComp, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	void Explode();



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
