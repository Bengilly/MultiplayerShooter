// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameMode.generated.h"

//forward delcare enum class in gamestate
enum class EEnemyWaveState : uint8;

/**
 * 
 */
UCLASS()
class SHOOTER_API ASGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	ASGameMode();
	
protected:

	//  ------------ Variables ------------  //

	FTimerHandle TimerHandle_EnemySpawner;
	FTimerHandle TimerHandle_NextWaveStart;;
	int32 EnemiesToSpawn;
	int32 WaveCount;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	float TimeBetweenEnemyWaves;


	//  ------------ Functions ------------  //

	void SetEnemyWaveState(EEnemyWaveState NewState);

	//implemented in blueprints to spawn enemy
	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void SpawnEnemy();

	//calls blueprint to spawn enemies and hardcodes when enemy wave ends
	void SpawnEnemyTimerElapsed();

	//start spawning enemies
	void SpawnEnemyWave();

	//stop spawning enemies
	void EndEnemyWave();

	//timer for next enemy wave
	void StartTimerForNextWave();

	//handle when to start next wave
	void QueryWaveState();


	void ScanForAlivePlayers();

	void GameOver();

public:

	virtual void StartPlay() override;
	virtual void Tick(float DeltaSeconds) override;
};
