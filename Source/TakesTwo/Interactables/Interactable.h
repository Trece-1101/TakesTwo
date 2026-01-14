// Copyright 2020-2026 NiceBug Games All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

class TAKESTWO_API IInteractable
{
	GENERATED_BODY()

public:
	virtual void ActivateInteraction() = 0;
	virtual void DeactivateInteraction() = 0;
};
