// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TakesTwoGameMode.generated.h"

/**
 *  Simple GameMode for a third person game
 */
UCLASS(abstract)
class ATakesTwoGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:	
	ATakesTwoGameMode();
	
	virtual void StartPlay() override;
	
protected:
	void UpdateMatchTimer();

	FTimerHandle TimerHandle_MatchCountdown;

	UPROPERTY(EditAnywhere, Category = "Rules")
	int32 MatchDuration = 60;
};



