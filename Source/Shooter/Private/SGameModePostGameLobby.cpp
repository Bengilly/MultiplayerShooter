// Fill out your copyright notice in the Description page of Project Settings.
#include "SGameModePostGameLobby.h"
#include "SGameState.h"
#include "SPlayerState.h"
#include "SPlayerControllerLobby.h"

ASGameModePostGameLobby::ASGameModePostGameLobby()
{

}

//void ASGameModePostGameLobby::BeginPlay()
//{
//	Super::BeginPlay();
//
//	GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Green, FString::Printf(TEXT("Player connected to postgame lobby")));
//
//	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
//	{
//		if (ASPlayerControllerLobby* LobbyPC = Cast<ASPlayerControllerLobby>(*It))
//		{
//
//			LobbyPC->ClientCreatePostgameLobbyMenu();
//		}
//	}
//}

//void ASGameModePostGameLobby::PostLogin(APlayerController* NewPlayerController)
//{
//
//}
//
//void ASGameModePostGameLobby::Logout(AController* PlayerController)
//{
//
//}