// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "SGameMode.h"
#include "Net/UnrealNetwork.h"

ASPlayerController::ASPlayerController()
{
}

void ASPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		SpawnPlayer();
	}
	
}

void ASPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}

void ASPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASPlayerController::SpawnPlayer()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		//only trigger once for owner of this controller
		ASGameMode* GameMode = Cast<ASGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		GameMode->SpawnPlayer(this);
	}
	else
	{
		ServerSpawnPlayer();
	}


}

void ASPlayerController::ServerSpawnPlayer_Implementation()
{
	SpawnPlayer();
}
