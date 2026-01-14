// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/** Main log category used across the project */
DECLARE_LOG_CATEGORY_EXTERN(LogTakesTwo, Log, All);

static void LogOnScreen(const UObject* WorldContext, const FString& Msg, FColor Color = FColor::White, float Duration = 5.0f)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);

	// Net Prefix is helpful during PIE
	FString FullMessage = World->IsNetMode(NM_Client) ? "[CLIENT] " : "[SERVER] " + Msg;
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, FullMessage);
	}

	UE_LOG(LogTakesTwo, Log, TEXT("%s"), *FullMessage);
}