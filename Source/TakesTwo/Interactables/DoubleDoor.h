// Copyright 2020-2026 NiceBug Games All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "GameFramework/Actor.h"
#include "DoubleDoor.generated.h"

UCLASS()
class TAKESTWO_API ADoubleDoor : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	ADoubleDoor();
	
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void ActivateInteraction() override;
	virtual void DeactivateInteraction() override;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> LeftHinge;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> LeftDoorMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> RightHinge;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> RightDoorMesh;
	
	UPROPERTY(EditAnywhere, Category = "Config")
	float TargetYawAngle = 90.0f; // Cuánto abren

	UPROPERTY(EditAnywhere, Category = "Config")
	float RotationSpeed = 4.0f;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	int32 TriggersToOpen = 1;
	
private:
	int32 ActiveTriggers = 0;

	UPROPERTY(Replicated)
	bool bIsDoorOpen;

};
