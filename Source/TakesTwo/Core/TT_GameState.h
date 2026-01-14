// Copyright 2020-2026 NiceBug Games All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "TT_GameState.generated.h"

/**
 * 
 */
UCLASS()
class TAKESTWO_API ATT_GameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	ATT_GameState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game Info")
	int32 TimeRemaining = 60;
};
