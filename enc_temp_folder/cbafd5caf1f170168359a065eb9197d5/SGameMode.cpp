// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameMode.h"
#include "TimerManager.h"
#include "Components/SHealthComponent.h"
#include "EngineUtils.h"
#include "SGameState.h"
#include "SPlayerState.h"
#include "SCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerController.h"


ASGameMode::ASGameMode()
{
	GameStateClass = ASGameState::StaticClass();
	PlayerStateClass = ASPlayerState::StaticClass();

	//set tickinterval to 1 second to avoid calling every frame
	PrimaryActorTick.TickInterval = 1.0f;
	PrimaryActorTick.bCanEverTick = true;

	//// set default pawn class to our Blueprinted character
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Content/Blueprints/BP_Character"));
	//if (PlayerPawnBPClass.Class != NULL)
	//{
	//	DefaultPawnClass = PlayerPawnBPClass.Class;
	//}

	//TimeBetweenEnemyWaves = 2.0f;
}

void ASGameMode::BeginPlay()
{
	Super::BeginPlay();

	
}

void ASGameMode::StartPlay()
{
	Super::StartPlay();

	//SpawnPlayersAtSpawnPoints();


	//StartTimerForNextWave();
}

void ASGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//QueryWaveState();
	//ScanForAlivePlayers();
}


void ASGameMode::SpawnPlayer(APlayerController* PlayerController)
{
	//check if player already possesses pawn and destroy it
	APawn* PlayerPawn = PlayerController->GetPawn();
	if (IsValid(PlayerPawn))
	{
		PlayerPawn->Destroy();
	}

	PlayerPawn = GetWorld()->SpawnActor<APawn>(PlayerClass, FindRandomSpawnLocation());

	PlayerController->Possess(PlayerPawn);

}

//find all instances of the Player Start class and assign random spawn location
FTransform ASGameMode::FindRandomSpawnLocation() const
{
	//TSubclassOf<AActor> PlayerStartActors;
	TArray<AActor*> SpawnPointArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), SpawnPointArray);

	if (SpawnPointArray.Num() > 0)
	{
		//find a random spawn point between 0 and total available player start locations
		int32 SpawnPointIndex = FMath::RandRange(0, SpawnPointArray.Num() - 1);

		return SpawnPointArray[SpawnPointIndex]->GetActorTransform();
	}
	return FTransform::Identity;
}

////spawn players at selected player start instances		/*	needs further work	*/
//void ASGameMode::SpawnPlayersAtSpawnPoints()
//{
//	UE_LOG(LogTemp, Warning, TEXT("Spawning players..."));
//
//	//find all instances of the Player Start class
//	TSubclassOf<APlayerStart> PlayerStartActors;
//	TArray<AActor*> SpawnPointArray;
//	UGameplayStatics::GetAllActorsOfClass(GetWorld(), PlayerStartActors, SpawnPointArray);
//
//	// iterate over found spawn points and spawn players
//	int32 PlayerIndex = 0;
//	for (AActor* SpawnPoint : SpawnPointArray)
//	{
//		APlayerStart* PlayerStart = Cast<APlayerStart>(SpawnPoint);
//		if (PlayerStart)
//		{
//			// spawn a player at the current spawn point
//			APlayerController* PC = UGameplayStatics::CreatePlayer(GetWorld(), PlayerIndex, true);
//			if (PC)
//			{
//				APawn* NewPlayerPawn = GetWorld()->SpawnActor<APawn>(DefaultPawnClass, PlayerStart->GetActorLocation(), PlayerStart->GetActorRotation());
//				if (NewPlayerPawn)
//				{
//					PC->Possess(NewPlayerPawn);
//				}
//			}
//
//			// Increment player index for the next spawn point
//			PlayerIndex++;
//		}
//	}
//}


void ASGameMode::SetGameState(EGameState NewState)
{
	ASGameState* GS = GetGameState<ASGameState>();
	{
		if (GS)
		{
			//call function on server to replicate to clients about the updating gamestate
			GS->SetState(NewState);
			
		}
	}
}




//void ASGameMode::SetEnemyWaveState(EEnemyWaveState NewState)
//{
//	ASGameState* GS = GetGameState<ASGameState>();
//	{
//		if (GS)
//		{
//			//call function on server to replicate to clients about the updating gamestate
//			GS->SetWaveState(NewState);
//			
//		}
//	}
//}
//
//void ASGameMode::SpawnEnemyWave()
//{
//	WaveCount++;
//	EnemiesToSpawn = 2 * WaveCount;
//
//	GetWorldTimerManager().SetTimer(TimerHandle_EnemySpawner, this, &ASGameMode::SpawnEnemyTimerElapsed, 1.0f, true, 0.0f);
//
//	SetEnemyWaveState(EEnemyWaveState::SpawningWave);
//}
//
//void ASGameMode::SpawnEnemyTimerElapsed()
//{
//	//spawning implemented in blueprint
//	SpawnEnemy();
//
//	EnemiesToSpawn--;
//	if (EnemiesToSpawn <= 0)
//	{
//		EndEnemyWave();
//	}
//}
//
//void ASGameMode::EndEnemyWave()
//{
//	GetWorldTimerManager().ClearTimer(TimerHandle_EnemySpawner);
//
//	SetEnemyWaveState(EEnemyWaveState::FinishedSpawning);
//}
//
//
//void ASGameMode::StartTimerForNextWave()
//{
//	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &ASGameMode::SpawnEnemyWave, TimeBetweenEnemyWaves, false);
//
//	RespawnDeadPlayers();
//
//	SetEnemyWaveState(EEnemyWaveState::WaitingToSpawn);
//}
//
//
//void ASGameMode::QueryWaveState()
//{
//	bool bTimerStartedForNextWave = GetWorldTimerManager().IsTimerActive(TimerHandle_NextWaveStart);
//
//	//if there are still enemies to spawn or timer has started for next wave to spawn, ignore rest of query
//	if (bTimerStartedForNextWave || EnemiesToSpawn > 0)
//	{
//		return;
//	}
//
//	bool bAllEnemiesDead = true;
//
//	//find all enemy pawns
//	for (TActorIterator<APawn> It(GetWorld()); It; ++It)
//	{
//		APawn* PlayerPawn = *It;
//		if (PlayerPawn == nullptr || PlayerPawn->IsPlayerControlled())
//		{
//			continue;
//		}
//
//		//check if there are any enemies still alive in the wave
//		USHealthComponent* HealthComponent = Cast<USHealthComponent>(PlayerPawn->GetComponentByClass(USHealthComponent::StaticClass()));
//		if (HealthComponent && HealthComponent->GetHealth() > 0.0f)
//		{
//			bAllEnemiesDead = false;
//			break;
//		}
//	}
//
//	//if all bots are dead, start timer for next wave to spawn
//	if (bAllEnemiesDead)
//	{
//		StartTimerForNextWave();
//
//		SetEnemyWaveState(EEnemyWaveState::WaveComplete);
//	}
//}
//
//void ASGameMode::ScanForAlivePlayers()
//{
//	//find if there are any alive players
//	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
//	{
//		APlayerController* PC = It->Get();
//		if (PC && PC->GetPawn())
//		{
//			APawn* MyPawn = PC->GetPawn();
//			USHealthComponent* HealthComponent = Cast<USHealthComponent>(MyPawn->GetComponentByClass(USHealthComponent::StaticClass()));
//			if (HealthComponent && HealthComponent->GetHealth() > 0.0f)
//			{
//				//player still alive
//				return;
//			}
//		}
//	}
//
//	//if no players are alive, end game
//	GameOver();
//}
//
//
//void ASGameMode::GameOver()
//{
//	EndEnemyWave();
//	SetEnemyWaveState(EEnemyWaveState::GameOver);
//
//	UE_LOG(LogTemp, Log, TEXT("Game Over"));
//}
//
//
//void ASGameMode::RespawnDeadPlayers()
//{
//	//find if there are any alive players and respawn them - as GameMode runs on server, all player controllers are available
//	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
//	{
//		APlayerController* PC = It->Get();
//
//		//if getpawn = null, then player = dead, so respawn (when player dies, they un-possess the pawn causing it to be null)
//		if (PC && PC->GetPawn() == nullptr)
//		{
//			RestartPlayer(PC);
//		}
//
//	}
//}