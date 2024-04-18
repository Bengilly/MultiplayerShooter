// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "SPlayerProfile.h"
#include "SSaveGamePlayerProfile.h"
#include "SGameInstance.generated.h"


class USSaveGamePlayerProfile;
struct FSPlayerProfileStruct;
class FOnlineSessionSearchResult;

USTRUCT(BlueprintType)
struct FSSessionSearchResults
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	int32 ResultSessionIndex;

	UPROPERTY(BlueprintReadOnly)
	FString ResultSessionName;

	UPROPERTY(BlueprintReadOnly)
	int32 NumOpenSlots;

	UPROPERTY(BlueprintReadOnly)
	int32 NumMaxSlots;

	UPROPERTY(BlueprintReadOnly)
	int32 Ping;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerDelegate, FSSessionSearchResults, ServerListDelegate);


UCLASS()
class SHOOTER_API USGameInstance : public UGameInstance
{
	GENERATED_BODY()

	USGameInstance();

public:

	UPROPERTY(BlueprintReadWrite)
	int MaxPlayers;

	UPROPERTY(BlueprintReadWrite)
	FName SessionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSPlayerProfileStruct PlayerProfileStruct;

	UPROPERTY(BlueprintReadOnly, Category = "Session Results")
	FSSessionSearchResults SessionSearchResults;

	UPROPERTY(BlueprintReadOnly, Category = "Session Results")
	TArray<FSSessionSearchResults> SessionSearchResultsArray;


protected:

	//  ------------ Variables ------------  //

	FString PlayerProfileSlot;
	USSaveGamePlayerProfile* SGPlayerProfile;
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSearch> OnlineSessionSearch;
	TArray<FOnlineSessionSearchResult> SearchResults;

	UPROPERTY(BlueprintAssignable)
	FServerDelegate ServerListDelegate;


	UPROPERTY(BlueprintReadWrite)
	bool bIsLanEnabled;

	//  ------------ Functions ------------  //

	virtual void Init() override;
	virtual void OnCreateSessionComplete(FName Name, bool bSucceeded);
	virtual void OnFindSessionComplete(bool bSucceeded);
	virtual void OnJoinSessionComplete(FName Name, EOnJoinSessionCompleteResult::Type Result);

	UFUNCTION(BlueprintCallable)
	void CreateMultiplayerSession();

	UFUNCTION(BlueprintCallable)
	TArray<FSSessionSearchResults> FindMultiplayerSession();

	UFUNCTION(BlueprintCallable)
	void JoinSession(int32 SessionIndex);

	void SavePlayerProfile();

	USSaveGamePlayerProfile* LoadPlayerProfile();

	UFUNCTION(BlueprintCallable)
	void ChangePlayerName(FString PlayerName);

	UFUNCTION(BlueprintCallable)
	void CheckForSavedProfile();

	//  ------------ UI ------------  //
};
