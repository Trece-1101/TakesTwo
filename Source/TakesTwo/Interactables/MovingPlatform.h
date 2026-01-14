// Copyright 2020-2026 NiceBug Games All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "GameFramework/Actor.h"
#include "MovingPlatform.generated.h"

UCLASS()
class TAKESTWO_API AMovingPlatform : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AMovingPlatform();
	virtual void Tick(float DeltaTime) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void ActivateInteraction() override;
	virtual void DeactivateInteraction() override;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;
		
	UPROPERTY(EditAnywhere, Category = "Config", Meta = (MakeEditWidget = true))
	FVector TargetLocationRelative;

	UPROPERTY(EditAnywhere, Category = "Config")
	float MoveSpeed = 200.0f;
	
	UPROPERTY(EditAnywhere, Category = "Config")
	int32 TriggerToActivate = 1;

private:
	FVector GlobalStartLocation;
	FVector GlobalTargetLocation;

	int32 ActiveTriggers = 0;
    
	UPROPERTY(Replicated)
	bool bShouldMoveToEnd;
};
