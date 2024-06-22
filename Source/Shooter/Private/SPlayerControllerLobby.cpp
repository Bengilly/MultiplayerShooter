// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerControllerLobby.h"
#include "Kismet/GameplayStatics.h"
#include "SSaveGamePlayerProfile.h"
#include "SGameModeLobby.h"
#include "Net/UnrealNetwork.h"

ASPlayerControllerLobby::ASPlayerControllerLobby()
{
	
}

void ASPlayerControllerLobby::LoadSavedProfile()
{
	USaveGame* SaveGame;
	FString LocalPlayerProfileSlot = "PlayerProfileSlot";

	if (!UGameplayStatics::DoesSaveGameExist(LocalPlayerProfileSlot, 0))
	{
		SaveGame = Cast<USSaveGamePlayerProfile>(UGameplayStatics::CreateSaveGameObject(USSaveGamePlayerProfile::StaticClass()));
		UGameplayStatics::SaveGameToSlot(SaveGame, LocalPlayerProfileSlot, 0);
	}

	SaveGame = UGameplayStatics::LoadGameFromSlot(LocalPlayerProfileSlot, 0);

	USSaveGamePlayerProfile* PlayerSaveGame = Cast<USSaveGamePlayerProfile>(SaveGame);
	PlayerProfileStruct = PlayerSaveGame->PlayerProfileInfoStruct;
	//GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Green, FString::Printf(TEXT("(PlayerControllerLobby) Player profile loaded")));
}

//Server - Update lobby widget with connected player names
void ASPlayerControllerLobby::ServerUpdateLobby_Implementation(FSPlayerProfileStruct PlayerProfile)
{
	PlayerProfileStruct = PlayerProfile;

	ASGameModeLobby* GM = Cast<ASGameModeLobby>(GetWorld()->GetAuthGameMode());
	GM->UpdateLobby(true);
	//GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Green, FString::Printf(TEXT("ServerUpdateLobby called")));
}

//calls blueprint function on client to load lobby menu widget
void ASPlayerControllerLobby::ClientCreateLobbyMenu_Implementation()
{
	ShowLobbyMenu();
}

//calls blueprint function on client to load loading screen widget
void ASPlayerControllerLobby::ClientShowLoadingScreen_Implementation()
{
	ShowLoadingScreen();
}

//Client - Load saved profile and tell server to update lobby list
void ASPlayerControllerLobby::ClientInitialPlayerSetup_Implementation()
{ 
	LoadSavedProfile();
	ServerUpdateLobby(PlayerProfileStruct);
}

void ASPlayerControllerLobby::ClientUpdatePlayerNames_Implementation(const TArray<FSPlayerProfileStruct>& AllPlayerProfileStructs)
{
	UpdatePlayerNames(AllPlayerProfileStructs);
}