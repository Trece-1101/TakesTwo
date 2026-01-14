// Copyright 2020-2026 NiceBug Games All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PressurePlate.generated.h"

class UNiagaraSystem;
class USoundBase;

UCLASS()
class TAKESTWO_API APressurePlate : public AActor
{
	GENERATED_BODY()

public:
	APressurePlate();
	
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> PlateMesh;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BaseMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> TriggerMesh;
	
	UPROPERTY(EditAnywhere, Category = "Config")
	FVector TriggerScalePadding;
	
	UPROPERTY(EditAnywhere, Category = "Config")
	FVector TriggerLocationOffset;
	
	UPROPERTY(EditAnywhere, Category = "Interaction")
	bool bIsOneShot = false;
	
	UPROPERTY(EditInstanceOnly, Category = "Interaction")
	TArray<AActor*> InteractableLinkedActors;	
	
	UPROPERTY(EditAnywhere, Category = "Interaction")
	int32 ActivationThreshold = 1;

	UPROPERTY(EditAnywhere, Category = "Config")
	float DepressionDepth = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Config")
	float InterpSpeed = 5.0f;
	
	// FX	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX")
	TObjectPtr<USoundBase> ActivationSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX")
	TObjectPtr<USoundBase> DeactivationSound;

	UPROPERTY(EditAnywhere, Category = "FX")
	TObjectPtr<UNiagaraSystem> ActivationParticles;
	
	UFUNCTION(NetMulticast, Unreliable) 
	void Multicast_PlayEffects(bool bIsActive);

private:
	UPROPERTY()
	TSet<AActor*> OverlappingActors;
	
	UPROPERTY(ReplicatedUsing = OnRep_IsActivated)
	bool bIsActivated;
	
	float InitialPlateZ;
	
	UFUNCTION()
	void OnRep_IsActivated();
	
	bool IsValidActivator(const AActor* ActorToCheck);
	void CheckActivationCondition();
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	

};
