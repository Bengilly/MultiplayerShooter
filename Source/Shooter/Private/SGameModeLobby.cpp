// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeLobby.h"
#include "SGameState.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "SPlayerController.h"
#include "SPlayerControllerLobby.h"
#include "SGameInstance.h"

ASGameModeLobby::ASGameModeLobby()
{
	GameStateClass = ASGameState::StaticClass();

	WarmupDuration = 15.0f;
}

void ASGameModeLobby::BeginPlay()
{
	Super::BeginPlay();
}

//set the new game state and replicate to other clients
void ASGameModeLobby::SetGameState(EGameState NewState)
{
	ASGameState* GS = GetGameState<ASGameState>();
	{
		if (GS)
		{
			//call function on server to replicate to clients about the updating gamestate
			GS->SetState(NewState);
		}
	}
}

//Warmup timers
void ASGameModeLobby::StartWarmup()
{
	SetGameState(EGameState::WaitingToStart);

	//start timer for warmup
	GetWorld()->GetTimerManager().SetTimer(TimerHandler_WarmupTimer, this, &ASGameModeLobby::WarmupTimerInterval, 1.0f, true, 0);
}

void ASGameModeLobby::WarmupTimerInterval()
{
	WarmupDuration -= 1.0f;

	UE_LOG(LogTemp, Log, TEXT("Remaining warmup time: %f"), WarmupDuration);

	ASGameState* GS = GetGameState<ASGameState>();
	GS->UpdateWarmupTimerToPlayers(WarmupDuration);

	if (WarmupDuration <= 0.0f)
	{
		//gameover
		UE_LOG(LogTemp, Log, TEXT("Warmup has ended, game on!"));

		GetWorldTimerManager().ClearTimer(TimerHandler_WarmupTimer);

		ServerTravelToMap("Level_Forest");
	}
}

//load new level on the server
void ASGameModeLobby::ServerTravelToMap(const FString& MapName)
{
	// future work - notify clients before travel
	// future work - handle game state rep here

	// perform server travel to load forest level
	UWorld* World = GetWorld();
	if (World)
	{
		FString URL = FString::Printf(TEXT("/Game/Maps/%s?listen"), *MapName);
		World->ServerTravel(URL, true, false);
	}
}

//add the player controller to the array when they join the game session
void ASGameModeLobby::PostLogin(APlayerController* NewPlayerController)
{
	Super::PostLogin(NewPlayerController);

	ASPlayerControllerLobby* LobbyPC = Cast<ASPlayerControllerLobby>(NewPlayerController);
	UE_LOG(LogTemp, Log, TEXT("PostLogin Controller connected: %s"), *FString(LobbyPC->GetName()));

	if (LobbyPC)
	{
		ConnectedPlayersArray.Add(LobbyPC);
	}

	//update the number of max players on the lobby screen
	USGameInstance* GI = Cast<USGameInstance>(GetGameInstance());
	MaxConnectedPlayers = GI->MaxPlayers;

	ASGameState* GS = GetGameState<ASGameState>();
	GS->UpdateMaxPlayerCount(MaxConnectedPlayers);
	
	//start game once all players have connected to the session
	if (MaxConnectedPlayers == ConnectedPlayersArray.Num())
	{
		StartWarmup();
	}
	
}

//remove the player controller from the array when they disconnect
void ASGameModeLobby::Logout(AController* PlayerController)
{
	Super::Logout(PlayerController);

	ASPlayerControllerLobby* LeavingPlayerController = Cast<ASPlayerControllerLobby>(PlayerController);
	if (LeavingPlayerController)
	{
		ConnectedPlayersArray.Remove(LeavingPlayerController);
	}
}