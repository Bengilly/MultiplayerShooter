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

	if (IsLocalPlayerController())
	{
		ServerSpawnPlayerCharacter();
	}
}

void ASPlayerController::SpawnPlayerCharacter()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerSpawnPlayerCharacter();
	}

	UE_LOG(LogTemp, Log, TEXT("(Spawning) Spawning player..."));
	ASGameMode* GM = Cast<ASGameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		GM->SpawnPlayer(this, bIsRespawn);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("(Spawning) Gamemode is null..."));
	}
}

void ASPlayerController::SetIsRespawn(bool IsRespawn)
{
	bIsRespawn = IsRespawn;
}

void ASPlayerController::ServerSpawnPlayerCharacter_Implementation()
{
	SpawnPlayerCharacter();
}

//enable controller input for owning client
void ASPlayerController::ClientEnablePlayerInput_Implementation()
{
	this->GetPawn()->EnableInput(this);
}

void ASPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}

void ASPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}