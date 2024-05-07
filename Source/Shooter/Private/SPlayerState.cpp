// Fill out your copyright notice in the Description page of Project Settings.

#include "SPlayerState.h"
#include "Net/UnrealNetwork.h"

void ASPlayerState::UpdateScore(float ScoreToAdd)
{
	//Score += ScoreToAdd;
	Score = GetScore();
	SetScore(Score += ScoreToAdd);
}

void ASPlayerState::UpdateDeaths()
{
	Deaths += 1;
}

float ASPlayerState::GetDeaths() const
{
	return Deaths;
}

void ASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//replicate variables
	DOREPLIFETIME(ASPlayerState, Deaths);
}