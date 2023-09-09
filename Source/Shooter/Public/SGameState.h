// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SGameState.generated.h"

UENUM(BlueprintType)
enum class EEnemyWaveState : uint8
{
	WaitingToSpawn,
	Spawning,
	Active,
	Complete,
	GameOver,

};

/**
 * 
 */
UCLASS()
class SHOOTER_API ASGameState : public AGameStateBase
{
	GENERATED_BODY()
	
protected:

	UFUNCTION()
	void OnRep_WaveState(EEnemyWaveState PreviousState);

	UFUNCTION(BlueprintImplementableEvent, Category = "GameState")
	void WaveStateUpdated(EEnemyWaveState NewState, EEnemyWaveState PreviousState);

public:

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_WaveState, Category = "GameState")
	EEnemyWaveState WaveState;

};
