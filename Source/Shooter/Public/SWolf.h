// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SWolf.generated.h"

class ASCharacter;
class USHealthComponent;

UCLASS()
class SHOOTER_API ASWolf : public ACharacter
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWolf();

	void OnDeath();

protected:

	//  ------------ Variables ------------  //

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USHealthComponent* HealthComponent;

	//wolf damage (customizable)
	UPROPERTY(EditDefaultsOnly, Category = "Wolf")
	float WolfDamage;

	//patrol speed
	UPROPERTY(EditDefaultsOnly, Category = "Wolf")
	float WalkSpeed;

	//chase speed
	UPROPERTY(EditDefaultsOnly, Category = "Wolf")
	float RunSpeed;

	//reference to the player character
	ASCharacter* PlayerCharacter;

	//flag for detecting if the wolf is currently chasing the player, updates animation
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Wolf")
	bool bIsChasing;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Wolf")
	bool bIsWalking;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Wolf")
	bool bIsRunning;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Wolf")
	bool bIsBiting;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Wolf")
	bool bIsDead;


	//  ------------ Functions ------------  //

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//patrol function
	void Patrol(float DeltaTime);

	//chase player
	void ChasePlayer();

	//attack player
	UFUNCTION(BlueprintCallable)
	void AttackPlayer(ASCharacter* PlayerToAttack);


	//  ------------ Multiplayer Functions ------------  //

	UFUNCTION(Server, Reliable)
	void ServerAttackPlayer(ASCharacter* PlayerToAttack);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
