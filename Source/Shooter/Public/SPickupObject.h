// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SEnumAbilityPickup.h"
#include "SPickupObject.generated.h"

class USphereComponent;
class UDecalComponent;
class ASPowerupObject;

UCLASS()
class SHOOTER_API ASPickupObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPickupObject();

protected:

	//  ------------ Variables ------------  //

	ASPowerupObject* AbilityPickupInstance;

	UPROPERTY(EditInstanceOnly, Category = "PickupObject")
	float SpawnCooldown;

	UPROPERTY(EditDefaultsOnly, Category = "PickupObject")
	TArray<TSubclassOf<ASPowerupObject>> AbilityPickupType;

	FTimerHandle TimerHandle_RespawnTimer;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UDecalComponent* DecalComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* SphereComponent;


	//  ------------ Functions ------------  //

	void SpawnPickup();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
