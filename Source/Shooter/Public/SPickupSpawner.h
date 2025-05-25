// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Landscape.h"
#include "SPickupSpawner.generated.h"

class ASPickupObject;
class ASPowerupObject;

UCLASS()
class SHOOTER_API ASPickupSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPickupSpawner();

protected:

	//  ------------ Variables ------------  //

	AActor* ActorInstance;

	// area bounds for random point generation
	FVector Origin;
	FVector BoxExtent;

	UPROPERTY(EditInstanceOnly, Category = "Spawning")
	ALandscape* Landscape;

	// Number of actors to spawn
	UPROPERTY(EditInstanceOnly, Category = "Spawning")
	int32 NumberOfActorsToSpawn = 100;

	//set actor to spawn for each instance of the spawner
	UPROPERTY(EditInstanceOnly, Category = "Spawning")
	TSubclassOf<AActor> ActorToSpawn;

	//  ------------ Functions ------------  //
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SpawnPickupObjects();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};