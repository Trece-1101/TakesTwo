// Copyright 2020-2026 NiceBug Games All Rights Reserved.


#include "TT_GameState.h"
#include "Net/UnrealNetwork.h"

ATT_GameState::ATT_GameState()
{
}

void ATT_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ATT_GameState, TimeRemaining);
}
