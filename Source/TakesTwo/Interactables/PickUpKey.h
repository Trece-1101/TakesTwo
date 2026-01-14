// Copyright 2020-2026 NiceBug Games All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickUpKey.generated.h"

class USphereComponent;

UCLASS()
class TAKESTWO_API APickUpKey : public AActor
{
	GENERATED_BODY()

public:
	APickUpKey();
	
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void OnPickedUp();

protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> TriggerSphere;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float RotationSpeed = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float BobbingSpeed = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float BobbingHeight = 20.0f;

	FVector InitialLocation;
	
	UPROPERTY(ReplicatedUsing = OnRep_IsPickedUp)
	bool bIsPickedUp = false;
	
	UFUNCTION()
	void OnRep_IsPickedUp();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
						bool bFromSweep, const FHitResult& SweepResult);
};
