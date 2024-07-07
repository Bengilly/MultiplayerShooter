// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerProfile.h"
#include "SPlayerControllerLobby.generated.h"


/**
 * 
 */

struct FSPlayerProfileStruct;

UCLASS()
class SHOOTER_API ASPlayerControllerLobby : public APlayerController
{
	GENERATED_BODY()
	
public:

	//  ------------ Variables ------------  //

	FSPlayerProfileStruct PlayerProfileStruct;

	//  ------------ Functions ------------  //

	UFUNCTION(Client, Unreliable)
	void ClientInitialPlayerSetup();
	
	UFUNCTION(Client, Reliable)
	void ClientUpdatePlayerNames(const TArray<FSPlayerProfileStruct>& AllPlayerProfileStructs);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdatePlayerNames(const TArray<FSPlayerProfileStruct>& AllPlayerProfileStructs);

	//Show lobby menu on client PC
	UFUNCTION(Client, Unreliable)
	void ClientCreateLobbyMenu();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowLobbyMenu();

	//Show postgame lobby menu on client PC
	UFUNCTION(Client, Unreliable)
	void ClientCreatePostgameLobbyMenu();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowPostgameLobbyMenu();

	//Show loading screen on client PC 
	UFUNCTION(Client, Unreliable)
	void ClientShowLoadingScreen();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowLoadingScreen();

protected:

	//  ------------ Variables ------------  //



	//  ------------ Functions ------------  //

	ASPlayerControllerLobby();

	UFUNCTION(BlueprintCallable)
	void LoadSavedProfile();

	UFUNCTION(Server,Reliable, BlueprintCallable)
	void ServerUpdateLobby(FSPlayerProfileStruct PlayerProfile);

};
