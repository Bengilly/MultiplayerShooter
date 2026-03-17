// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

/**
 * 
 */

UCLASS()
class SHOOTER_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:

	ASPlayerState();

	virtual void CopyProperties(APlayerState* PlayerState) override;

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void SetPlayerScore(float ScoreToAdd);

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	int GetPlayerScore() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void SetTotalPlayerKills();

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	int GetTotalPlayerKills() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void SetTotalPlayerDeaths();

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	int GetTotalPlayerDeaths() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void SetCustomPlayerName(FString Name);

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	FString GetCustomPlayerName() const;

protected:

	UPROPERTY(Replicated)
	int PlayerScore;

	UPROPERTY(Replicated)
	int Deaths;

	UPROPERTY(Replicated)
	int Kills;

	UPROPERTY(Replicated)
	FString PlayerName;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
};