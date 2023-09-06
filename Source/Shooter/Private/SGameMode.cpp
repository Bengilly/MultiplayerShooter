// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameMode.h"
#include "TimerManager.h"
#include "Components/SHealthComponent.h"
#include "EngineUtils.h"


ASGameMode::ASGameMode()
{
	//set tickinterval to 1 second to avoid calling every frame
	PrimaryActorTick.TickInterval = 1.0f;
	PrimaryActorTick.bCanEverTick = true;

	TimeBetweenEnemyWaves = 2.0f;
}


void ASGameMode::StartPlay()
{
	Super::StartPlay();

	StartTimerForNextWave();
}

void ASGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	QueryWaveState();
}

void ASGameMode::SpawnEnemyWave()
{
	WaveCount++;

	EnemiesToSpawn = 2 * WaveCount;

	GetWorldTimerManager().SetTimer(TimerHandle_EnemySpawner, this, &ASGameMode::SpawnEnemyTimerElapsed, 1.0f, true, 0.0f);
}

void ASGameMode::SpawnEnemyTimerElapsed()
{
	//spawning implemented in blueprint
	SpawnEnemy();

	EnemiesToSpawn--;
	if (EnemiesToSpawn <= 0)
	{
		EndEnemyWave();
	}
}

void ASGameMode::EndEnemyWave()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_EnemySpawner);

}


void ASGameMode::StartTimerForNextWave()
{
	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &ASGameMode::SpawnEnemyWave, TimeBetweenEnemyWaves, false);
}

void ASGameMode::QueryWaveState()
{
	bool bTimerStartedForNextWave = GetWorldTimerManager().IsTimerActive(TimerHandle_NextWaveStart);

	//if there are still enemies to spawn or timer has started for next wave to spawn, ignore rest of query
	if (bTimerStartedForNextWave || EnemiesToSpawn > 0)
	{
		return;
	}

	bool bAllEnemiesDead = true;

	//find all enemy pawns
	for (TActorIterator<APawn> It(GetWorld()); It; ++It)
	{
		APawn* TestPawn = *It;
		if (TestPawn == nullptr || TestPawn->IsPlayerControlled())
		{
			continue;
		}

		//check if there are any enemies still alive in the wave
		USHealthComponent* HealthComponent = Cast<USHealthComponent>(TestPawn->GetComponentByClass(USHealthComponent::StaticClass()));
		if (HealthComponent && HealthComponent->GetHealth() > 0.0f)
		{
			bAllEnemiesDead = false;
			break;
		}
	}

	//if all bots are dead, start timer for next wave to spawn
	if (bAllEnemiesDead)
	{
		StartTimerForNextWave();
	}
}
