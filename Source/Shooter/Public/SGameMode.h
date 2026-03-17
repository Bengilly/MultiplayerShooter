// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameMode.generated.h"

class APlayerStart;
class ASCharacter;
class ASPlayerController;
class ASPlayerState;

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

	void SpawnPlayer(ASPlayerController* PlayerController, bool bIsRespawn);

	//UPROPERTY(BlueprintAssignable, Category = "GameMode")
	FOnActorKilled OnActorKilled;

protected:

	//  ------------ Variables ------------  //

	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TSubclassOf<ASCharacter> PlayerPawnClass;

	TSet<FVector> UsedSpawnLocations;
	FTimerHandle TimerHandler_GameTimer;
	FTimerHandle TimerHandler_WarmupTimer;
	FTimerHandle TimerHandler_FreezeTimer;
	FTimerHandle TimerHandler_RespawnTimer;
	FTimerHandle TimerHandler_FlagTimer;

	TArray<ASPlayerController*> ConnectedPlayersArray;


	//duration of the match in seconds
	UPROPERTY(Replicated, BlueprintReadOnly, EditDefaultsOnly)
	float MatchDuration;

	UPROPERTY(Replicated, BlueprintReadOnly, EditDefaultsOnly)
	float FreezeDuration;

	UPROPERTY(Replicated, BlueprintReadOnly, EditDefaultsOnly)
	float RespawnTimer;

	UPROPERTY(Replicated, BlueprintReadOnly, EditDefaultsOnly)
	float TimeWithFlag;

	int	ScoreIncrement;

	//  ------------ Functions ------------  //

	virtual void StartPlay() override;
	void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	void StartMatch();
	void SetPlayerInput(bool bEnableInput);
	void StartFreezeTimer();
	void FreezeTimerInterval();
	void MatchTimerInterval();
	void SetGameState(EGameState NewState);

	//timers
	void RespawnAllDeadPlayers();
	void StartRespawnTimer();
	void RespawnTimerInterval();

	UFUNCTION()
	void FlagTimerInterval(AActor* PickupActor);

	FTransform FindRandomSpawnLocation();

	void ServerTravelToMap(const FString& MapName);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UFUNCTION()
	void PlayerPickedUpFlag(ASCharacter* PickupActor, ASFlag* FlagActor);

	UFUNCTION()
	void PlayerDroppedFlag(ASCharacter* PickupActor, ASFlag* FlagActor);

	UFUNCTION()
	void OnPlayerKilled(AActor* EnemyKilled, AActor* DamagingActor, AController* DamagingActorController);

};
