// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameState.h"
#include "Net/UnrealNetwork.h"

//called from game mode to update the match timer. Timer displayed to player HUD.
void ASGameState::UpdateMatchTimerToPlayers(float Time)
{
	MatchTimer = Time;
}

void ASGameState::UpdateWarmupTimerToPlayers(float Time)
{
	WarmupTimer = Time;
}

void ASGameState::UpdateMaxPlayerCount(int NumOfPlayers)
{
	MaxPlayerCount = NumOfPlayers;
}

void ASGameState::UpdateFreezeTimerToPlayers(float Time)
{
	FreezeTimer = Time;
}

//  ------------ Multiplayer Functions ------------  //

void ASGameState::OnRep_GameState(EGameState PreviousState)
{
	WaveStateUpdated(GameState, PreviousState);
}

void ASGameState::SetState(EGameState NewState)
{
	//call on server
	if (GetLocalRole() == ROLE_Authority)
	{
		EGameState PreviousState = GameState;
		GameState = NewState;
		OnRep_GameState(PreviousState);
	}
}


void ASGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//replicate variables
	DOREPLIFETIME(ASGameState, GameState);
	DOREPLIFETIME(ASGameState, FreezeTimer)
	DOREPLIFETIME(ASGameState, MatchTimer)
	DOREPLIFETIME(ASGameState, WarmupTimer);
	DOREPLIFETIME(ASGameState, MaxPlayerCount);
}

//void ASGameState::OnRep_WaveState(EEnemyWaveState PreviousState)
//{
//	WaveStateUpdated(WaveState, PreviousState);
//}
//
//void ASGameState::SetWaveState(EEnemyWaveState NewState)
//{
//	//call on server
//	if (GetLocalRole() == ROLE_Authority)
//	{
//		EEnemyWaveState PreviousState = WaveState;
//		WaveState = NewState;
//		OnRep_WaveState(PreviousState);
//	}
//}