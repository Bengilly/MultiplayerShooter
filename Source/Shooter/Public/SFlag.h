// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SFlag.generated.h"

class ASCharacter;
class URotatingMovementComponent;
class UStaticMeshComponent;

UCLASS()
class SHOOTER_API ASFlag : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASFlag();
	ASCharacter* GetFlagHolder();

	void OnFlagPickedUp(ASCharacter* NewFlagHolder);
	void OnDropped();

protected:

	//  ------------ Variables ------------  //

	UPROPERTY(Replicated)
	ASCharacter* FlagHolder;

	UPROPERTY(VisibleAnywhere, Category = "Flag Components")
	URotatingMovementComponent* RotatingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Flag Components")
	UStaticMeshComponent* MeshComponent;


	//  ------------ Functions ------------  //

	virtual void BeginPlay() override;
	void NotifyActorBeginOverlap(AActor* Player);
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



};
