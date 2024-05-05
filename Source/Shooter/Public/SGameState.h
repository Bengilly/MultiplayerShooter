// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SGameState.generated.h"

//UENUM(BlueprintType)
//enum class EEnemyWaveState : uint8
//{
//	WaitingToSpawn,
//	SpawningWave,
//	FinishedSpawning,
//	WaveComplete,
//	GameOver,
//
//};

UENUM(BlueprintType)
enum class EGameState : uint8
{
	WaitingToStart,
	InProgress,
	GameOver
};

/**
 * 
 */
UCLASS()
class SHOOTER_API ASGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:

	//  ------------ Variables ------------  //

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_GameState, Category = "GameState")
	EGameState GameState;

	//  ------------ Functions ------------  //

	void SetState(EGameState NewState);

	UFUNCTION(BlueprintImplementableEvent, Category = "GameState")
	void WaveStateUpdated(EGameState NewState, EGameState PreviousState);

	UFUNCTION(Category = "GameState")
	void UpdateWarmupTimerToPlayers(float Time);

	UFUNCTION(Category = "GameState")
	void UpdateFreezeTimerToPlayers(float Time);

	UFUNCTION(Category = "GameState")
	void UpdateMatchTimerToPlayers(float Time);

	UFUNCTION(Category = "GameState")
	void UpdateRespawnTimerToPlayers(float Time);

	UFUNCTION(BlueprintCallable, Category = "GameState")
	void UpdateMaxPlayerCount(int Players);

protected:

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "GameState")
	float MatchTimer;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "GameState")
	float FreezeTimer;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "GameState")
	float WarmupTimer;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "GameState")
	float RespawnTimer;
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "GameState")
	int MaxPlayerCount;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UFUNCTION()
	void OnRep_GameState(EGameState PreviousState);



// 	UFUNCTION()
//	void OnRep_WaveState(EGameState PreviousState);
// 
//	UFUNCTION(BlueprintImplementableEvent, Category = "GameState")
//	void WaveStateUpdated(EEnemyWaveState NewState, EEnemyWaveState PreviousState);
//
//	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_WaveState, Category = "GameState")
//	EEnemyWaveState WaveState;
//
//public:
//
//	void SetWaveState(EEnemyWaveState NewState);


};
