// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameModePostGameLobby.generated.h"

class ASPlayerControllerLobby;

/**
 * 
 */
UCLASS()
class SHOOTER_API ASGameModePostGameLobby : public AGameModeBase
{
	GENERATED_BODY()
	

public:


protected:

	//  ------------ Functions ------------  //

	ASGameModePostGameLobby();

	//virtual void BeginPlay() override;
	//virtual void PostLogin(APlayerController* NewPlayerController) override;
	//virtual void Logout(AController* PlayerController) override;

};
