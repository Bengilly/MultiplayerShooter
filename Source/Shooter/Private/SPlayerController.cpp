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
		UE_LOG(LogTemp, Log, TEXT("(Spawning) Print Controller: %s"), *FString(this->GetName()));
		SpawnPlayerCharacter();
	}
}

void ASPlayerController::SpawnPlayerCharacter()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerSpawnPlayerCharacter();
	}

	UE_LOG(LogTemp, Log, TEXT("(Spawning) Spawning player..."));
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
	UE_LOG(LogTemp, Log, TEXT("(Spawning) Input boolean value: %s"), bEnableInput ? TEXT("True") : TEXT("False"));
	UE_LOG(LogTemp, Log, TEXT("(Spawning) Apply input change to: %s"), *FString(this->GetName()));

	SetIgnoreMoveInput(!bEnableInput);
	SetIgnoreLookInput(!bEnableInput);
}

void ASPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("ToggleInput", IE_Pressed, this, &ASPlayerController::ManuallyToggleInput);
}


void ASPlayerController::ManuallyToggleInput()
{
	TogglePlayerInput(false);
}


void ASPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}