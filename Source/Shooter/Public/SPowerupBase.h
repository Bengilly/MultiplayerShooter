// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPowerupBase.generated.h"

class USphereComponent;
class UDecalComponent;

UCLASS()
class SHOOTER_API ASPowerupBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPowerupBase();

protected:

	//  ------------ Variables ------------  //

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UDecalComponent* DecalComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* SphereComponent;


	
	//  ------------ Functions ------------  //
	// 
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
