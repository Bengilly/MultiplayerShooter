// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerControllerPostLobby.h"

void ASPlayerControllerPostLobby::BeginPlay()
{
    Super::BeginPlay();

    if (IsLocalController())
    {
        ShowPostgameLobbyMenu();
    }
}