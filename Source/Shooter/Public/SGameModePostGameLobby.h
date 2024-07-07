// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameModePostGameLobby.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER_API ASGameModePostGameLobby : public AGameModeBase
{
	GENERATED_BODY()
	
	ASGameModePostGameLobby();


protected:

	//void BeginPlay() override;

	virtual void PostLogin(APlayerController* NewPlayerController) override;
	virtual void Logout(AController* PlayerController) override;

};
