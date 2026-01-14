// Copyright 2020-2026 NiceBug Games All Rights Reserved.


#include "PressurePlate.h"

#include "Interactable.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"


APressurePlate::APressurePlate()
{
	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates = true;
	SetReplicateMovement(false);
	bIsActivated = false;
	
	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	RootComponent = SceneRoot;
	
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("BaseMesh");
	BaseMesh->SetupAttachment(RootComponent);
	BaseMesh->SetCollisionProfileName(FName("BlockAll"));
	
	PlateMesh = CreateDefaultSubobject<UStaticMeshComponent>("PlateMesh");
	PlateMesh->SetupAttachment(BaseMesh);
	PlateMesh->SetCollisionProfileName(FName("BlockAll"));
	
	TriggerMesh = CreateDefaultSubobject<UStaticMeshComponent>("TriggerMesh");
	TriggerMesh->SetupAttachment(PlateMesh);
	TriggerMesh->SetCollisionProfileName(FName("Trigger"));
	TriggerMesh->SetHiddenInGame(true);
	TriggerMesh->SetVisibility(true);
	TriggerMesh->SetCastShadow(false);
	TriggerMesh->SetComponentTickEnabled(false);
	
	TriggerScalePadding = FVector(1.02f, 1.02f, 4.0f);
	TriggerLocationOffset = FVector(0.0f, 0.0f, 10.0f); 
}


void APressurePlate::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	if (PlateMesh && TriggerMesh)
	{
		UStaticMesh* TriggerMeshAsset = PlateMesh->GetStaticMesh();
		
		if (TriggerMeshAsset)
		{
			TriggerMesh->SetStaticMesh(TriggerMeshAsset);
			TriggerMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		
		TriggerMesh->SetRelativeScale3D(TriggerScalePadding);
		TriggerMesh->SetRelativeLocation(TriggerLocationOffset);
		
#if WITH_EDITOR
		if (TriggerMesh)
		{
			UMaterialInterface* DebugMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/EngineDebugMaterials/LevelColorationLitMaterial"));
            
			if (DebugMat)
			{
				const int32 MatCount = TriggerMesh->GetNumMaterials();
				for (int32 i = 0; i < MatCount; i++)
				{
					TriggerMesh->SetMaterial(i, DebugMat);
				}
			}
		}
#endif
	}
}

void APressurePlate::BeginPlay()
{
	Super::BeginPlay();
	
	InitialPlateZ = PlateMesh->GetRelativeLocation().Z;
	
	if (HasAuthority())
	{
		TriggerMesh->OnComponentBeginOverlap.AddDynamic(this, &APressurePlate::OnOverlapBegin);
		TriggerMesh->OnComponentEndOverlap.AddDynamic(this, &APressurePlate::OnOverlapEnd);
	}
}

void APressurePlate::Multicast_PlayEffects_Implementation(bool bIsActive)
{
	if (bIsActive && ActivationParticles)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), 
			ActivationParticles, 
			GetActorLocation()
		);
	}
}

void APressurePlate::OnRep_IsActivated()
{
	if (bIsActivated)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ActivationSound, GetActorLocation());
	}
	else
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeactivationSound, GetActorLocation());
	}
}

void APressurePlate::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(APressurePlate, bIsActivated);
}

void APressurePlate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	float TargetZ = bIsActivated ? (InitialPlateZ - DepressionDepth) : InitialPlateZ;
	FVector CurrentLocation = PlateMesh->GetRelativeLocation();

	if (!FMath::IsNearlyEqual(CurrentLocation.Z, TargetZ, 0.1f))
	{
		float NewZ = FMath::FInterpTo(CurrentLocation.Z, TargetZ, DeltaTime, InterpSpeed);
		PlateMesh->SetRelativeLocation(FVector(CurrentLocation.X, CurrentLocation.Y, NewZ));
	}
}

void APressurePlate::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority() || !OtherActor || OtherActor == this) return;
	
	if (IsValidActivator(OtherActor))
	{
		OverlappingActors.Add(OtherActor);
		CheckActivationCondition();
	}
}

void APressurePlate::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{	
	if (!HasAuthority() || !OtherActor) return;
	
	if (OverlappingActors.Contains(OtherActor))
	{
		OverlappingActors.Remove(OtherActor);
		CheckActivationCondition();
	}
}

bool APressurePlate::IsValidActivator(const AActor* ActorToCheck)
{
	if (!ActorToCheck) return false;

	bool bIsCharacter = ActorToCheck->IsA(ACharacter::StaticClass());

	bool bIsWeightedObject = ActorToCheck->ActorHasTag(FName("Weight"));
	
	//LogOnScreen(this, FString::Printf(TEXT("Actor %s"), *ActorToCheck->GetName()), FColor::White);

	return bIsCharacter || bIsWeightedObject;
}

void APressurePlate::CheckActivationCondition()
{
	bool bShouldActivate = OverlappingActors.Num() >= ActivationThreshold;
	
	if (bIsOneShot && bIsActivated)
	{
		bShouldActivate = true;
	}
	
	if (bIsActivated != bShouldActivate)
	{
		bIsActivated = bShouldActivate;
		OnRep_IsActivated();
		
		if (HasAuthority())
		{
			for (AActor* Actor : InteractableLinkedActors)
			{
				if (Actor && Actor->Implements<UInteractable>())
				{
					IInteractable* Interface = Cast<IInteractable>(Actor);
					if (bIsActivated)
					{
						Interface->ActivateInteraction();
						Multicast_PlayEffects(bIsActivated);
					}
					else
					{
						Interface->DeactivateInteraction();
					}
				}
			}
		}
	}
}





