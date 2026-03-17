// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerControllerPostLobby.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER_API ASPlayerControllerPostLobby : public APlayerController
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent)
	void ShowPostgameLobbyMenu();
	
private:
	virtual void BeginPlay() override;


};
