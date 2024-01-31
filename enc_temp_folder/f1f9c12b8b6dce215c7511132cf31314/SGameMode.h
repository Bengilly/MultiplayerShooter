// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameMode.generated.h"

class APlayerStart;
class ASCharacter;

//forward delcare enum class in gamestate
enum class EGameState : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilled, AActor*, EnemyKilled, AActor*, DamagingActor, AController*, DamagingActorController);

/**
 * 
 */

UCLASS()
class SHOOTER_API ASGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	ASGameMode();

	void SpawnPlayer(APlayerController* PlayerController);
	
protected:

	//  ------------ Variables ------------  //

	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TSubclassOf<ASCharacter> PlayerClass;




	//  ------------ Functions ------------  //

	//void SpawnPlayersAtSpawnPoints();

	FTransform FindRandomSpawnLocation() const;



	void SetGameState(EGameState NewState);
















	//  ------------ Variables ------------  //

	//FTimerHandle TimerHandle_EnemySpawner;
	//FTimerHandle TimerHandle_NextWaveStart;;
	//int32 EnemiesToSpawn;
	//int32 WaveCount;

	//UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	//float TimeBetweenEnemyWaves;

	//  ------------ Functions ------------  //

	//void SetEnemyWaveState(EEnemyWaveState NewState);

	////implemented in blueprints to spawn enemy
	//UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	//void SpawnEnemy();

	////calls blueprint to spawn enemies and hardcodes when enemy wave ends
	//void SpawnEnemyTimerElapsed();

	////start spawning enemies
	//void SpawnEnemyWave();

	////stop spawning enemies
	//void EndEnemyWave();

	////timer for next enemy wave
	//void StartTimerForNextWave();

	////respawn dead players
	//void RespawnDeadPlayers();

	////handle when to start next wave
	//void QueryWaveState();


	//void ScanForAlivePlayers();

	//void GameOver();

public:

	virtual void StartPlay() override;
	void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;





	UPROPERTY(BlueprintAssignable, Category = "GameMode")
	FOnActorKilled OnActorKilled;
};
