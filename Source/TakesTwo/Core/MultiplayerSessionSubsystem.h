// Copyright 2020-2026 NiceBug Games All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MultiplayerSessionSubsystem.generated.h"

UCLASS()
class TAKESTWO_API UMultiplayerSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UMultiplayerSessionSubsystem();
	
	// IGameInstanceSubsystem Interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	// Public API
	/** Creates a session and travels to the lobby map */
	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	void CreateServer(const FString& ServerName, const FString& MapPath);
	
	/** Finds sessions and auto-joins if name matches */
	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	void FindServer(const FString& ServerName);
	
private:
	// Internal Callbacks
	void OnCreateSessionComplete(FName SessionName, bool Success);
	void OnDestroySessionComplete(FName SessionName, bool Success);
	void OnFindSessionsComplete(const bool Success);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	
	// Member Variables
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	
	// Delegate Handles
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle DestroySessionCompleteDelegateHandle;
	FDelegateHandle FindSessionsCompleteDelegateHandle;
	FDelegateHandle JoinSessionCompleteDelegateHandle;
	
	// State Variables
	bool bCreateSessionOnDestroy = false;
	FName MySessionName;
	FString ServerNameToFind;
	FString LastServerName;
	FString DesiredMapPath;
	FString LastMapPath;
};
