// Copyright 2020-2026 NiceBug Games All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "GameFramework/Actor.h"
#include "Door.generated.h"

UCLASS()
class TAKESTWO_API ADoor : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	ADoor();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;
	
	virtual void ActivateInteraction() override;
	virtual void DeactivateInteraction() override;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> HingeComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> DoorMesh;
	
	UPROPERTY(EditAnywhere, Category = "Config")
	float TargetYawAngle = 90.0f;

	UPROPERTY(EditAnywhere, Category = "Config")
	float RotationSpeed = 4.0f;

	UPROPERTY(EditAnywhere, Category = "Config")
	int32 TriggersToOpen = 1;

private:
	FRotator ClosedRotation;
	FRotator OpenRotation;

	int32 ActiveTriggers = 0;

	UPROPERTY(Replicated)
	bool bIsDoorOpen;
};
