// Fill out your copyright notice in the Description page of Project Settings.

#include "SPlayerState.h"
#include "Net/UnrealNetwork.h"



ASPlayerState::ASPlayerState()
{
	Kills = 0;
	Deaths = 0;
	PlayerScore = 0;
}

//called when server travels to new map, copies player state info to new player state object created for new level
void ASPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	ASPlayerState* NewPS = Cast<ASPlayerState>(PlayerState);

	if (NewPS)
	{
		NewPS->PlayerScore = GetPlayerScore();
		NewPS->Kills = GetTotalPlayerKills();
		NewPS->Deaths = GetTotalPlayerDeaths();
	}

}

//create separate score variable to copy over to postgame scoreboard as it wasn't working with engine score variable
void ASPlayerState::SetPlayerScore(float ScoreToAdd)
{
	PlayerScore = GetScore();
	SetScore(PlayerScore + ScoreToAdd);

	UE_LOG(LogTemp, Log, TEXT("Player Score: %f"), Score);
}

int ASPlayerState::GetPlayerScore() const
{
	return PlayerScore;
}

void ASPlayerState::SetTotalPlayerKills()
{
	Kills += 1;
}

int ASPlayerState::GetTotalPlayerKills() const
{
	return Kills;
}

void ASPlayerState::SetTotalPlayerDeaths()
{
	Deaths += 1;
}

int ASPlayerState::GetTotalPlayerDeaths() const
{
	return Deaths;
}

void ASPlayerState::SetCustomPlayerName(FString Name)
{
	PlayerName = Name;
	//GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Green, FString::Printf(TEXT("SetPlayerName: %s"), *PlayerName));
}

FString ASPlayerState::GetCustomPlayerName() const
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Green, FString::Printf(TEXT("GetPlayerName: %s"), *PlayerName));
	return PlayerName;
}

void ASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//replicate variables
	DOREPLIFETIME(ASPlayerState, PlayerName);
	DOREPLIFETIME(ASPlayerState, Deaths);
	DOREPLIFETIME(ASPlayerState, Kills);
	DOREPLIFETIME(ASPlayerState, PlayerScore);
}