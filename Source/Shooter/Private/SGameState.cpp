// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameState.h"
#include "Net/UnrealNetwork.h"

//  ------------ Multiplayer Functions ------------  //

void ASGameState::OnRep_WaveState(EEnemyWaveState PreviousState)
{
	WaveStateUpdated(WaveState, PreviousState);
}

void ASGameState::SetWaveState(EEnemyWaveState NewState)
{
	//call on server
	if (GetLocalRole() == ROLE_Authority)
	{
		EEnemyWaveState PreviousState = WaveState;
		WaveState = NewState;
		OnRep_WaveState(PreviousState);
	}
}

void ASGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//replicate variables
	DOREPLIFETIME(ASGameState, WaveState);
}