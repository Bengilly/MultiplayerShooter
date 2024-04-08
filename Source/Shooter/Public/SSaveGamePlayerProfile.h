// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SPlayerProfile.h"
#include "SSaveGamePlayerProfile.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER_API USSaveGamePlayerProfile : public USaveGame
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSPlayerProfileStruct PlayerProfileInfoStruct;
};