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

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void UpdateScore(float ScoreToAdd);

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void SetTotalPlayerDeaths();

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	float GetTotalPlayerDeaths() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void SetCustomPlayerName(FString Name);

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	FString GetCustomPlayerName() const;

protected:

	UPROPERTY(Replicated)
	float Deaths;

	UPROPERTY(Replicated)
	FString PlayerName;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
};