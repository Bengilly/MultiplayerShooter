// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameMode.generated.h"

class APlayerStart;
class ASCharacter;
class ASPlayerController;

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

	void SpawnPlayer(ASPlayerController* PlayerController);

	UFUNCTION(BlueprintPure, Category = "Match Timer")
	float GetRemainingMatchTime() const;

	
protected:

	//  ------------ Variables ------------  //

	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TSubclassOf<ASCharacter> PlayerPawnClass;

	TSet<FVector> UsedSpawnLocations;
	FTimerHandle TimerHandler_GameTimer;
	FTimerHandle TimerHandler_WarmupTimer;

	TArray<ASPlayerController*> ConnectedPlayersArray;


	//duration of the match in seconds
	UPROPERTY(Replicated, BlueprintReadOnly, EditDefaultsOnly)
	float MatchDuration;

	//duration of the match in seconds
	UPROPERTY(Replicated, BlueprintReadOnly, EditDefaultsOnly)
	float WarmupDuration;

	//  ------------ Functions ------------  //

	//void StartWarmup();
	//void WarmupTimerInterval();

	void StartMatch();
	void MatchTimerInterval();

	void SetGameState(EGameState NewState);

	FTransform FindRandomSpawnLocation();
	
	void ToggleControllerInput(TArray<ASPlayerController*> PlayerArray, bool bEnableInput);
	void ToggleControllerInput(ASPlayerController* PlayerController, bool bEnableInput);
	
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;



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

	//respawn dead players
	void RespawnDeadPlayers();

	////handle when to start next wave
	//void QueryWaveState();


	//void ScanForAlivePlayers();

	//void GameOver();

public:

	virtual void StartPlay() override;
	void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	//virtual void PostLogin(APlayerController* NewPlayerController) override;
	//virtual void Logout(AController* PlayerController) override;

	UPROPERTY(BlueprintAssignable, Category = "GameMode")
	FOnActorKilled OnActorKilled;
};
