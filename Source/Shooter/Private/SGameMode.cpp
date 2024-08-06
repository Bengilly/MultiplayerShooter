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
#include "Components/ArrowComponent.h"
#include "Net/UnrealNetwork.h"
#include "SPlayerController.h"

ASGameMode::ASGameMode()
{
	GameStateClass = ASGameState::StaticClass();
	PlayerStateClass = ASPlayerState::StaticClass();
	PlayerControllerClass = ASPlayerController::StaticClass();

	//set tickinterval to 1 second to avoid calling every frame
	PrimaryActorTick.TickInterval = 1.0f;
	PrimaryActorTick.bCanEverTick = true;

	MatchDuration = 60.0f;
	FreezeDuration = 5.0f;
	RespawnTimer = 10.0f;
}

void ASGameMode::BeginPlay()
{
	Super::BeginPlay();

	ASGameState* GS = GetGameState<ASGameState>();
	GS->UpdateFreezeTimerToPlayers(FreezeDuration);
	GS->UpdateMatchTimerToPlayers(MatchDuration);
	GS->UpdateRespawnTimerToPlayers(RespawnTimer);

	StartFreezeTimer();
}

void ASGameMode::StartPlay()
{
	Super::StartPlay();

	//StartTimerForNextWave();
}

void ASGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//QueryWaveState();
	//ScanForAlivePlayers();
}

//set the new game state and replicate to other clients
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

//start freeze timer once level loads and update UI
void ASGameMode::StartFreezeTimer()
{
	GetWorld()->GetTimerManager().SetTimer(TimerHandler_FreezeTimer, this, &ASGameMode::FreezeTimerInterval, 1.0f, true, 0);
}

void ASGameMode::FreezeTimerInterval()
{
	FreezeDuration -= 1.0f;

	UE_LOG(LogTemp, Log, TEXT("Freeze time remaining: %f"), FreezeDuration);

	if (FreezeDuration <= 0.0f)
	{
		UE_LOG(LogTemp, Log, TEXT("Freeze time has ended!"));

		GetWorldTimerManager().ClearTimer(TimerHandler_FreezeTimer);

		StartMatch();
	}

	ASGameState* GS = GetGameState<ASGameState>();
	GS->UpdateFreezeTimerToPlayers(FreezeDuration);
}

void ASGameMode::StartMatch()
{
	SetGameState(EGameState::InProgress);
	StartRespawnTimer();
	SetPlayerInput(true);

	//empty spawn location array so they can be reused next spawn wave
	UsedSpawnLocations.Empty();

	//start timer for round
	GetWorld()->GetTimerManager().SetTimer(TimerHandler_GameTimer, this, &ASGameMode::MatchTimerInterval, 1.0f, true, 0);
}
 
//decrease match timer
void ASGameMode::MatchTimerInterval()
{
	MatchDuration -= 1.0f;

	UE_LOG(LogTemp, Log, TEXT("Remaining match time: %f"), MatchDuration);

	if (MatchDuration <= 0.0f)
	{
		//gameover
		UE_LOG(LogTemp, Log, TEXT("Match has ended!"));

		GetWorldTimerManager().ClearTimer(TimerHandler_GameTimer);

		//disable player input
		SetPlayerInput(false);

		SetGameState(EGameState::GameOver);
		ServerTravelToMap("Level_Lobby_PostGame");
	}

	ASGameState* GS = GetGameState<ASGameState>();
	GS->UpdateMatchTimerToPlayers(MatchDuration);
}

//handle the spawning of player pawn on the server
void ASGameMode::SpawnPlayer(ASPlayerController* PlayerController, bool IsRespawn)
{
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Log, TEXT("PlayerController not valid"));
		return;
	}

	//check if player already possesses pawn and destroy it
	APawn* PlayerPawn = PlayerController->GetPawn();
	if (IsValid(PlayerPawn))
	{
		PlayerPawn->Destroy();
	}

	APawn* NewPlayerPawn = GetWorld()->SpawnActor<APawn>(PlayerPawnClass, FindRandomSpawnLocation());
	if (IsValid(NewPlayerPawn))
	{
		PlayerController->Possess(NewPlayerPawn);

		if (IsRespawn)
		{
			PlayerController->ClientEnablePlayerInput();
		}

	}
}

void ASGameMode::SetPlayerInput(bool bEnableInput)
{
	if (UWorld* World = GetWorld())
	{
		// iterate over all player controllers and enable input
		for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
		{
			ASPlayerController* PC = Cast<ASPlayerController>(It->Get());
			if (PC && bEnableInput)
			{
				PC->ClientEnablePlayerInput();
			}
			else if (PC && !bEnableInput)
			{
				PC->ClientDisablePlayerInput();
			}
		}
	}
}

//find all instances of the Player Start class and assign random spawn location
FTransform ASGameMode::FindRandomSpawnLocation()
{
	TArray<AActor*> SpawnPointArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), SpawnPointArray);

	if (SpawnPointArray.Num() > 0)
	{
		TArray<FVector> AvailableSpawnLocations;

		for (AActor* SpawnPoint : SpawnPointArray)
		{
			FVector SpawnLocation = SpawnPoint->GetActorLocation();
			if (!UsedSpawnLocations.Contains(SpawnLocation))
			{
				AvailableSpawnLocations.Add(SpawnLocation);
			}
		}

		if (AvailableSpawnLocations.Num() > 0)
		{
			//find a random spawn point between 0 and total available player start locations
			int32 SpawnPointIndex = FMath::RandRange(0, AvailableSpawnLocations.Num() - 1);
			FVector SelectedSpawnLocation = AvailableSpawnLocations[SpawnPointIndex];

			UE_LOG(LogTemp, Log, TEXT("SpawnPointIndex: %d"), SpawnPointIndex);
			UE_LOG(LogTemp, Log, TEXT("SelectedSpawnLocation: %s"), *SelectedSpawnLocation.ToString());

			// add to used locations
			UsedSpawnLocations.Add(SelectedSpawnLocation);

			return FTransform(SelectedSpawnLocation);
		}
	}
	return FTransform::Identity;
}

void ASGameMode::StartRespawnTimer()
{
	GetWorld()->GetTimerManager().SetTimer(TimerHandler_RespawnTimer, this, &ASGameMode::RespawnTimerInterval, 1.0f, true, 0);
}

void ASGameMode::RespawnTimerInterval()
{
	RespawnTimer -= 1.0f;

	//GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Green, FString::Printf(TEXT("Respawn time: %d"), RespawnTimer));
	UE_LOG(LogTemp, Log, TEXT("Respawn time remaining: %f"), RespawnTimer);

	if (RespawnTimer <= 0.0f)
	{
		UE_LOG(LogTemp, Log, TEXT("Respawning players!"));
		GetWorldTimerManager().ClearTimer(TimerHandler_RespawnTimer);

		//uncomment RespawnAllDeadPlayers(); to add respawn on timer
		RespawnAllDeadPlayers();

		//empty spawn location array so they can be reused next spawn wave
		UsedSpawnLocations.Empty();

		//set and restart respawn timer
		RespawnTimer = 10.0;
		StartRespawnTimer();
	}

	ASGameState* GS = GetGameState<ASGameState>();
	GS->UpdateRespawnTimerToPlayers(RespawnTimer);
}

void ASGameMode::RespawnAllDeadPlayers()
{
	//find if there are any alive players and respawn them - as GameMode runs on server, all player controllers are available
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ASPlayerController* PC = Cast<ASPlayerController>(It->Get());

		//if getpawn = null, then player = dead, so respawn (when player dies, they un-possess the pawn causing it to be null)
		if (PC && PC->GetPawn() == nullptr)
		{
			PC->SetIsRespawn(true);
			PC->SpawnPlayerCharacter();
		}
	}
}

//load new level on the server
void ASGameMode::ServerTravelToMap(const FString& MapName)
{
	// perform server travel to new level
	UWorld* World = GetWorld();
	if (World)
	{
		FString URL = FString::Printf(TEXT("/Game/Maps/%s?listen"), *MapName);
		World->ServerTravel(URL, true, false);
	}
}

void ASGameMode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//replicate variables
	DOREPLIFETIME(ASGameMode, MatchDuration);
	DOREPLIFETIME(ASGameMode, RespawnTimer);
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