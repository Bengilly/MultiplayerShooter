// Fill out your copyright notice in the Description page of Project Settings.

#include "SPlayerState.h"
#include "Net/UnrealNetwork.h"



ASPlayerState::ASPlayerState()
{
	Deaths = 0;
}

void ASPlayerState::UpdateScore(float ScoreToAdd)
{
	Score = GetScore();
	SetScore(Score += ScoreToAdd);
}

void ASPlayerState::SetTotalPlayerDeaths()
{
	Deaths += 1;
}

float ASPlayerState::GetTotalPlayerDeaths() const
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
	GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Green, FString::Printf(TEXT("GetPlayerName: %s"), *PlayerName));
	return PlayerName;
}

void ASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//replicate variables
	DOREPLIFETIME(ASPlayerState, PlayerName);
	DOREPLIFETIME(ASPlayerState, Deaths);
}