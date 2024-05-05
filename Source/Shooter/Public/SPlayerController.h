// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASPlayerController();

	void SpawnPlayerCharacter();

	void SetIsRespawn(bool IsRespawn);

	UFUNCTION(Server, Reliable)
	void ServerSpawnPlayerCharacter();

	UFUNCTION(Client, Unreliable)
	void ClientEnablePlayerInput();


protected:

	bool bIsRespawn;


	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaTime) override;

	//  ------------ Multiplayer Functions ------------  //




};
