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
	//Super::BeginPlay();

	if (IsLocalPlayerController())
	{
		UE_LOG(LogTemp, Log, TEXT("(Spawning) Print Controller: %s"), *FString(this->GetName()));
		SpawnPlayerCharacter();
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

void ASPlayerController::SpawnPlayerCharacter()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerSpawnPlayerCharacter();
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("(Spawning) Spawning player..."));
	//only trigger once for owner of this controller
	ASGameMode* GameMode = Cast<ASGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->SpawnPlayer(this);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("(Spawning) Gamemode is null..."));
	}

}

void ASPlayerController::ServerSpawnPlayerCharacter_Implementation()
{
	SpawnPlayerCharacter();
}

void ASPlayerController::TogglePlayerInput(bool bEnableInput)
{

	// Print boolean value to the console
	UE_LOG(LogTemp, Log, TEXT("(Spawning) Input boolean value: %s"), bEnableInput ? TEXT("True") : TEXT("False"));
	UE_LOG(LogTemp, Log, TEXT("(Spawning) Apply input change to: %s"), *FString(this->GetName()));

	SetIgnoreMoveInput(!bEnableInput);
	SetIgnoreLookInput(!bEnableInput);
}

void ASPlayerController::ServerTogglePlayerInput_Implementation(bool bEnableInput)
{
	TogglePlayerInput(bEnableInput);
}
