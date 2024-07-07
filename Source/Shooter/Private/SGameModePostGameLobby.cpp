// Fill out your copyright notice in the Description page of Project Settings.
#include "SGameState.h"
#include "SPlayerState.h"
#include "SPlayerControllerLobby.h"
#include "SGameModePostGameLobby.h"


ASGameModePostGameLobby::ASGameModePostGameLobby()
{
	//GameStateClass = ASGameState::StaticClass();
	//PlayerStateClass = ASPlayerState::StaticClass();
	//PlayerControllerClass = ASPlayerControllerLobby::StaticClass();
}

//void ASGameModePostGameLobby::BeginPlay()
//{
//	Super::BeginPlay();
//
//
//}

void ASGameModePostGameLobby::PostLogin(APlayerController* NewPlayerController)
{
	Super::PostLogin(NewPlayerController);

	ASPlayerControllerLobby* LobbyPC = Cast<ASPlayerControllerLobby>(NewPlayerController);
	if (LobbyPC)
	{
		LobbyPC->ClientCreatePostgameLobbyMenu();
	}
}

void ASGameModePostGameLobby::Logout(AController* PlayerController)
{
}
