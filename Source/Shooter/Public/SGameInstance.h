// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER_API USGameInstance : public UGameInstance
{
	GENERATED_BODY()

	USGameInstance();

public:

	UPROPERTY(BlueprintReadWrite)
	int MaxPlayers;
};
