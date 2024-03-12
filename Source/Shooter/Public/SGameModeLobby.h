// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameModeLobby.generated.h"

/**
 * 
 */

//forward delcare enum class in gamestate
enum class EGameState : uint8;
class ASPlayerController;

UCLASS()
class SHOOTER_API ASGameModeLobby : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	//duration of the warmup in seconds
	float WarmupDuration;

	int MaxConnectedPlayers;

	UPROPERTY(BlueprintReadOnly)
	TArray<APlayerController*> ConnectedPlayersArray;
protected:

	ASGameModeLobby();

	void BeginPlay();

	void StartWarmup();

	void WarmupTimerInterval();

	void ServerTravelToMap(const FString& MapName);

	virtual void PostLogin(APlayerController* NewPlayerController) override;
	virtual void Logout(AController* PlayerController) override;

	void SetGameState(EGameState NewState);

	FTimerHandle TimerHandler_WarmupTimer;
};
