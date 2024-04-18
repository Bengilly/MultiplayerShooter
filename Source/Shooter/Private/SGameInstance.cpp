// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameInstance.h"
#include "Net/UnrealNetwork.h"
#include "SSaveGamePlayerProfile.h"
#include "SPlayerProfile.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Engine/World.h"

USGameInstance::USGameInstance()
{
	MaxPlayers = 2;
	PlayerProfileSlot = "PlayerProfileSlot";
}

void USGameInstance::Init()
{
	Super::Init();

	if (IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get())
	{
		SessionInterface = OnlineSubsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			//bind delegates
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &USGameInstance::OnCreateSessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &USGameInstance::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &USGameInstance::OnJoinSessionComplete);
		}
	}
}
 
void USGameInstance::CreateMultiplayerSession()
{
	UE_LOG(LogTemp, Log, TEXT("(Session) Creating multiplayer server..."));

	FOnlineSessionSettings SessionSettings;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bIsDedicated = false;		
	SessionSettings.bIsLANMatch = (IOnlineSubsystem::Get()->GetSubsystemName() != "NULL") ? false : true;		//set to false for local testing
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.NumPublicConnections = MaxPlayers;		//pull from UI

	SessionInterface->CreateSession(0, SessionName, SessionSettings);
}

//perform server travel to load level on session creation
void USGameInstance::OnCreateSessionComplete(FName Name, bool bSucceeded)
{
	UE_LOG(LogTemp, Log, TEXT("(Session) Session created successfully: %d"), bSucceeded);
	if (bSucceeded)
	{
		const FString& MapName = "Level_Lobby";
		if (UWorld* World = GetWorld())
		{
			FString URL = FString::Printf(TEXT("/Game/Maps/%s?listen"), *MapName);
			World->ServerTravel(URL, true, false);

			//GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Green, FString::Printf(TEXT("Loading Level_Lobby...")));
		}
	}
}

TArray<FSSessionSearchResults> USGameInstance::FindMultiplayerSession()
{
	UE_LOG(LogTemp, Log, TEXT("(Session) Finding multiplayer server..."));

	OnlineSessionSearch = MakeShareable(new FOnlineSessionSearch());
	OnlineSessionSearch->bIsLanQuery = true;
	OnlineSessionSearch->MaxSearchResults = 10000;
	OnlineSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	SessionInterface->FindSessions(0, OnlineSessionSearch.ToSharedRef());

	//GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Green, FString::Printf(TEXT("return SessionSearchResultsArray")));
	return SessionSearchResultsArray;
}

void USGameInstance::OnFindSessionComplete(bool bSucceeded)
{
	if (bSucceeded)
	{
		SearchResults = OnlineSessionSearch->SearchResults;

		//GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Green, FString::Printf(TEXT("Sessions found: %d"), SearchResults.Num()));

		if (SearchResults.Num())
		{
			int32 SessionResultIndex = 0;		//used as a unique identifier for each session so users connect to the correct one
			for (FOnlineSessionSearchResult SearchResult : SearchResults)
			{
				SessionSearchResults.ResultSessionIndex = SessionResultIndex;
				SessionSearchResults.ResultSessionName = SearchResult.Session.OwningUserName;
				SessionSearchResults.NumOpenSlots = SearchResult.Session.NumOpenPublicConnections;
				SessionSearchResults.NumMaxSlots = SearchResult.Session.SessionSettings.NumPublicConnections;
				SessionSearchResults.Ping = SearchResult.PingInMs;

				//GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Green, FString::Printf(TEXT("NumOpenSlots: %d"), SessionSearchResults.NumOpenSlots));
				//GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Green, FString::Printf(TEXT("NumMaxSlots: %d"), SessionSearchResults.NumMaxSlots));
				//GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Green, FString::Printf(TEXT("Ping: %d"), SessionSearchResults.Ping));
				//GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Green, FString::Printf(TEXT("ResultSessionName: %s"), *SessionSearchResults.ResultSessionName));

				SessionSearchResultsArray.Add(SessionSearchResults);
				ServerListDelegate.Broadcast(SessionSearchResults);
				SessionResultIndex += 1;
			}
		}
	}
}

//find which session has been selected from the UI and join
void USGameInstance::JoinSession(int32 SessionIndex)
{
	for (int i = 0; i < SearchResults.Num(); i++)
	{
		if (i == SessionIndex)
		{
			SessionInterface->JoinSession(0, FName(*SessionSearchResults.ResultSessionName), SearchResults[i]);
		}
	}	
}

void USGameInstance::OnJoinSessionComplete(FName Name, EOnJoinSessionCompleteResult::Type Result)
{
	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			FString ConnectionAddress = "";
			SessionInterface->GetResolvedConnectString(Name, ConnectionAddress);
			if (ConnectionAddress != "")
			{
				PC->ClientTravel(ConnectionAddress, ETravelType::TRAVEL_Absolute);
				//GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Green, FString::Printf(TEXT("Session joined")));
			}
		}
	}
}

void USGameInstance::ChangePlayerName(FString PlayerName)
{
	PlayerProfileStruct.PlayerName = PlayerName;

	UE_LOG(LogTemp, Log, TEXT("(Instance) New player name: %s"), *PlayerProfileStruct.PlayerName);
	SavePlayerProfile();
}

void USGameInstance::SavePlayerProfile()
{
	UE_LOG(LogTemp, Log, TEXT("(Instance) Save game"));

	//create savegame object if one does not already exist
	if (!IsValid(SGPlayerProfile))
	{
		UE_LOG(LogTemp, Log, TEXT("(Instance) SGPlayerProfile is not valid"));

		USaveGame* SG = UGameplayStatics::CreateSaveGameObject(USSaveGamePlayerProfile::StaticClass());
		SGPlayerProfile = Cast<USSaveGamePlayerProfile>(SG);
	}

	if (SGPlayerProfile)
	{
		UE_LOG(LogTemp, Log, TEXT("(Instance) Save new SGPlayerProfile to slot"));

		SGPlayerProfile->PlayerProfileInfoStruct = PlayerProfileStruct;
		UGameplayStatics::SaveGameToSlot(SGPlayerProfile,PlayerProfileSlot, 0);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("(Instance) Unable to save player profile - SGPlayerProfile is null"));
	}

}

USSaveGamePlayerProfile* USGameInstance::LoadPlayerProfile()
{
	//SGPlayerProfile = Cast<USSaveGamePlayerProfile>(UGameplayStatics::LoadGameFromSlot(PlayerProfileSlot, 0));

	USaveGame* SG = UGameplayStatics::LoadGameFromSlot(PlayerProfileSlot, 0);
	SGPlayerProfile = Cast<USSaveGamePlayerProfile>(SG);

	if (SGPlayerProfile)
	{
		PlayerProfileStruct = SGPlayerProfile->PlayerProfileInfoStruct;

		UE_LOG(LogTemp, Log, TEXT("(Instance) Loading profile with name: %s"), *PlayerProfileStruct.PlayerName);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("(Instance) Unable to save player profile - SGPlayerProfile is null"));
	}

	return SGPlayerProfile;
}

void USGameInstance::CheckForSavedProfile()
{
	if (UGameplayStatics::DoesSaveGameExist(PlayerProfileSlot, 0))
	{
		SGPlayerProfile = LoadPlayerProfile();
	}
	else
	{
		SavePlayerProfile();
	}
}