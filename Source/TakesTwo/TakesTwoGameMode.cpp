// Copyright Epic Games, Inc. All Rights Reserved.

#include "TakesTwoGameMode.h"

#include "TakesTwo.h"
#include "Core/TT_GameState.h"

ATakesTwoGameMode::ATakesTwoGameMode()
{
	// stub
}

void ATakesTwoGameMode::StartPlay()
{
	Super::StartPlay();
	
	ATT_GameState* MyGameState = GetGameState<ATT_GameState>();
	if (MyGameState)
	{
		MyGameState->TimeRemaining = MatchDuration;
	}

	GetWorldTimerManager().SetTimer(
		TimerHandle_MatchCountdown, 
		this, 
		&ATakesTwoGameMode::UpdateMatchTimer, 
		1.0f, 
		true
	);
}

void ATakesTwoGameMode::UpdateMatchTimer()
{
	ATT_GameState* MyGameState = GetGameState<ATT_GameState>();
	if (MyGameState)
	{
		MyGameState->TimeRemaining--;
		
		if (MyGameState->TimeRemaining <= 0)
		{
			MyGameState->TimeRemaining = 0;
			
			GetWorldTimerManager().ClearTimer(TimerHandle_MatchCountdown);
			
			//TODO: Logica y travel a Game Over
			LogOnScreen(this, "GAME OVER", FColor::Red, 10);
		}
	}
}
