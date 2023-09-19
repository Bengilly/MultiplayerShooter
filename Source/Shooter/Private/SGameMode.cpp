// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameMode.h"
#include "TimerManager.h"
#include "Components/SHealthComponent.h"
#include "EngineUtils.h"
#include "SGameState.h"
#include "SPlayerState.h"


ASGameMode::ASGameMode()
{
	GameStateClass = ASGameState::StaticClass();
	PlayerStateClass = ASPlayerState::StaticClass();

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

	ScanForAlivePlayers();
}

void ASGameMode::SetEnemyWaveState(EEnemyWaveState NewState)
{
	ASGameState* GS = GetGameState<ASGameState>();
	{
		if (GS)
		{
			//call function on server to replicate to clients about the updating gamestate
			GS->SetWaveState(NewState);
			
		}
	}
}

void ASGameMode::SpawnEnemyWave()
{
	WaveCount++;
	EnemiesToSpawn = 2 * WaveCount;

	GetWorldTimerManager().SetTimer(TimerHandle_EnemySpawner, this, &ASGameMode::SpawnEnemyTimerElapsed, 1.0f, true, 0.0f);

	SetEnemyWaveState(EEnemyWaveState::SpawningWave);
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

	SetEnemyWaveState(EEnemyWaveState::FinishedSpawning);
}


void ASGameMode::StartTimerForNextWave()
{
	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &ASGameMode::SpawnEnemyWave, TimeBetweenEnemyWaves, false);

	RespawnDeadPlayers();

	SetEnemyWaveState(EEnemyWaveState::WaitingToSpawn);
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
		APawn* PlayerPawn = *It;
		if (PlayerPawn == nullptr || PlayerPawn->IsPlayerControlled())
		{
			continue;
		}

		//check if there are any enemies still alive in the wave
		USHealthComponent* HealthComponent = Cast<USHealthComponent>(PlayerPawn->GetComponentByClass(USHealthComponent::StaticClass()));
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

		SetEnemyWaveState(EEnemyWaveState::WaveComplete);
	}
}

void ASGameMode::ScanForAlivePlayers()
{
	//find if there are any alive players
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && PC->GetPawn())
		{
			APawn* MyPawn = PC->GetPawn();
			USHealthComponent* HealthComponent = Cast<USHealthComponent>(MyPawn->GetComponentByClass(USHealthComponent::StaticClass()));
			if (HealthComponent && HealthComponent->GetHealth() > 0.0f)
			{
				//player still alive
				return;
			}
		}
	}

	//if no players are alive, end game
	GameOver();
}


void ASGameMode::GameOver()
{
	EndEnemyWave();
	SetEnemyWaveState(EEnemyWaveState::GameOver);

	UE_LOG(LogTemp, Log, TEXT("Game Over"));
}


void ASGameMode::RespawnDeadPlayers()
{
	//find if there are any alive players and respawn them - as GameMode runs on server, all player controllers are available
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();

		//if getpawn = null, then player = dead, so respawn (when player dies, they un-possess the pawn causing it to be null)
		if (PC && PC->GetPawn() == nullptr)
		{
			RestartPlayer(PC);
		}

	}
}