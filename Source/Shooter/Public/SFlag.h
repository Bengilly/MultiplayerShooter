// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SFlag.generated.h"

class ASCharacter;
class URotatingMovementComponent;
class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFlagPickedUp, ASCharacter*, PickupActor, ASFlag*, FlagActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFlagDropped, ASCharacter*, PickupActor, ASFlag*, FlagActor);

UCLASS()
class SHOOTER_API ASFlag : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASFlag();
	ASCharacter* GetFlagHolder();

	void OnDropped();

	FOnFlagPickedUp OnFlagPickedUp;
	FOnFlagDropped OnFlagDropped;

protected:

	//  ------------ Variables ------------  //

	UPROPERTY(Replicated)
	ASCharacter* FlagHolder;

	UPROPERTY(VisibleAnywhere, Category = "Flag")
	URotatingMovementComponent* RotatingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Flag")
	UStaticMeshComponent* MeshComponent;


	//  ------------ Functions ------------  //

	virtual void BeginPlay() override;
	void NotifyActorBeginOverlap(AActor* Player);
	void OnPickedUp(AActor* Player);


	//  ------------ Multiplayer ------------  //
	
	UFUNCTION(Server, Reliable)
	void ServerOnPickedUp(AActor* Player);

	UFUNCTION(Server, Reliable)
	void ServerOnDropped();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



};
