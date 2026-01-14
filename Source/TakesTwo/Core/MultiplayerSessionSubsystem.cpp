// Copyright 2020-2026 NiceBug Games All Rights Reserved.


#include "MultiplayerSessionSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemNames.h"
#include "TakesTwo.h"


UMultiplayerSessionSubsystem::UMultiplayerSessionSubsystem()
{
}

void UMultiplayerSessionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	IOnlineSubsystem* OnlineSubSystem = Online::GetSubsystem(GetWorld());
	
	if (OnlineSubSystem)
	{
		FName SubsystemName = OnlineSubSystem->GetSubsystemName();
		LogOnScreen(this, FString::Printf(TEXT("OSS: %s"), *SubsystemName.ToString()));
		
		SessionInterface = OnlineSubSystem->GetSessionInterface();
		
		if (SessionInterface.IsValid())
		{
			OnCreateSessionCompleteDelegateHandle = SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(
				this,
				&UMultiplayerSessionSubsystem::OnCreateSessionComplete
				);
			
			DestroySessionCompleteDelegateHandle = SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(
				this,
				&UMultiplayerSessionSubsystem::OnDestroySessionComplete
				);				
			
			JoinSessionCompleteDelegateHandle = SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(
				this,
				&UMultiplayerSessionSubsystem::OnJoinSessionComplete
				);
		}
	}
}

void UMultiplayerSessionSubsystem::Deinitialize()
{
	Super::Deinitialize();
	
	if (SessionInterface.IsValid())
	{
		SessionInterface->OnCreateSessionCompleteDelegates.Remove(OnCreateSessionCompleteDelegateHandle);
		SessionInterface->OnDestroySessionCompleteDelegates.Remove(DestroySessionCompleteDelegateHandle);
	}
}

void UMultiplayerSessionSubsystem::CreateServer(const FString& ServerName, const FString& MapPath)
{
	if (ServerName.IsEmpty())
	{
		LogOnScreen(this, "Server Name can't be emtpy", FColor::Red);
		return;
	}
	
	if (!SessionInterface.IsValid()) { return; }
	
	MySessionName = NAME_GameSession;
	FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(MySessionName);
	
	if (ExistingSession != nullptr)
	{
		bCreateSessionOnDestroy = true;
		LastServerName = ServerName;
		LastMapPath = MapPath;
		SessionInterface->DestroySession(MySessionName);
        
		LogOnScreen(this, "Destroying old session before creating new one...", FColor::Yellow);
		return;
	}
		
	DesiredMapPath = MapPath;
	
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bIsDedicated = false;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.NumPublicConnections = 2;
	SessionSettings.bUseLobbiesIfAvailable = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.bAllowJoinViaPresence = true;
	SessionSettings.bIsLANMatch = (Online::GetSubsystem(GetWorld())->GetSubsystemName() == NULL_SUBSYSTEM); 
	
	SessionSettings.Set(FName("SERVER_NAME"), ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	
	SessionInterface->CreateSession(0, MySessionName, SessionSettings);
	
	LogOnScreen(this, FString::Printf(TEXT("Create Server: %s"), *ServerName), FColor::Red);
}

void UMultiplayerSessionSubsystem::OnCreateSessionComplete(FName SessionName, const bool Success)
{
	if (Success)
	{
		if (!DesiredMapPath.IsEmpty())
		{
			FString TravelURL = FString::Printf(TEXT("%s?listen"), *DesiredMapPath);
			GetWorld()->ServerTravel(TravelURL);			
		}
	}
}

void UMultiplayerSessionSubsystem::FindServer(const FString& ServerName)
{
	if (ServerName.IsEmpty())
	{
		LogOnScreen(this, "Server Name can't be emtpy", FColor::Red);
		return;
	}
	
	if (!SessionInterface.IsValid()) { return; }
	
	ServerNameToFind = ServerName;
	
	FindSessionsCompleteDelegateHandle = SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(
		this,
		&UMultiplayerSessionSubsystem::OnFindSessionsComplete
		);
	
	//LogOnScreen(this, FString::Printf(TEXT("Joined Server: %s"), *ServerName), FColor::Red);
	SessionSearch = MakeShareable(new FOnlineSessionSearch);
	SessionSearch->bIsLanQuery = (Online::GetSubsystem(GetWorld())->GetSubsystemName() == NULL_SUBSYSTEM);
	SessionSearch->MaxSearchResults = 100;
	SessionSearch->QuerySettings.Set(FName("PRESENCESEARCH"), true, EOnlineComparisonOp::Equals);
		
	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	
	LogOnScreen(this, "Searching for sessions...", FColor::Cyan);
}

void UMultiplayerSessionSubsystem::OnFindSessionsComplete(const bool Success)
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->OnFindSessionsCompleteDelegates.Remove(FindSessionsCompleteDelegateHandle);
	}
	
	if (!Success || !SessionSearch.IsValid())
	{
		LogOnScreen(this, "Sessions Search Failed", FColor::Red);
		return;
	}
	
	LogOnScreen(this, FString::Printf(TEXT("Search found %d results"), SessionSearch->SearchResults.Num()), FColor::Cyan);
		
	for (const FOnlineSessionSearchResult& Result : SessionSearch->SearchResults)
	{
		FString FoundServerName;
		
		Result.Session.SessionSettings.Get(FName("SERVER_NAME"), FoundServerName);
		UE_LOG(LogTemp, Warning, TEXT("Session Found: ID=%s, Name=%s"), *Result.GetSessionIdStr(), *FoundServerName);
		
		if (FoundServerName.Equals(ServerNameToFind))
		{
			LogOnScreen(this, FString::Printf(TEXT("MATCH FOUND! Joining %s..."), *FoundServerName), FColor::Green);
			
			SessionInterface->JoinSession(0, MySessionName, Result);
			
			return;
		}
	}
	
	LogOnScreen(this, "Server not found in results", FColor::Orange);
}

void UMultiplayerSessionSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->OnJoinSessionCompleteDelegates.Remove(JoinSessionCompleteDelegateHandle);
	}
	
	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		LogOnScreen(this, "Join Session Failed!", FColor::Red);
		return;
	}
	
	FString Address;
	if (SessionInterface->GetResolvedConnectString(MySessionName, Address))
	{
		LogOnScreen(this, FString::Printf(TEXT("Connect String: %s"), *Address), FColor::Yellow);
		
		APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
		if (PlayerController)
		{
			PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
		}
	}
}

void UMultiplayerSessionSubsystem::OnDestroySessionComplete(FName SessionName, const bool Success)
{
	if (bCreateSessionOnDestroy)
	{
		bCreateSessionOnDestroy = false;
		
		if (Success)
		{
			LogOnScreen(this, "Session Destroyed. Creating New Session...", FColor::Green);
			CreateServer(LastServerName, LastMapPath);
		}
		else
		{
			LogOnScreen(this, "Failed to destroy session", FColor::Red);
		}
	}
}



