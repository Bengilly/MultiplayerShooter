// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedStruct.h"
#include "SPlayerProfile.generated.h"

/**
 * 
 */

 USTRUCT(BlueprintType)
 struct FSPlayerProfileStruct
 {
 	GENERATED_BODY()
 
 public:
 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
 	FString PlayerName;
 };