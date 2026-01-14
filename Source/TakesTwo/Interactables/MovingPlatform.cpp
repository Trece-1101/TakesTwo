// Copyright 2020-2026 NiceBug Games All Rights Reserved.


#include "MovingPlatform.h"

#include "Net/UnrealNetwork.h"


AMovingPlatform::AMovingPlatform()
{
	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates = true;
	SetReplicateMovement(true);
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("MeshComponent");
	RootComponent = MeshComponent;
	
	TargetLocationRelative = FVector(0.f, 0.f, 0.f);
}

void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();
	
	GlobalStartLocation = GetActorLocation();    
	GlobalTargetLocation = GetTransform().TransformPosition(TargetLocationRelative);
}

void AMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (HasAuthority()) 
	{
		FVector CurrentLocation = GetActorLocation();
		FVector Destination = bShouldMoveToEnd ? GlobalTargetLocation : GlobalStartLocation;
        
		if (CurrentLocation.Equals(Destination, 1.0f)) return;

		FVector NewLoc = FMath::VInterpConstantTo(CurrentLocation, Destination, DeltaTime, MoveSpeed);
		SetActorLocation(NewLoc);
	}
}

void AMovingPlatform::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AMovingPlatform, bShouldMoveToEnd);
}

void AMovingPlatform::ActivateInteraction()
{
	ActiveTriggers++;
    
	if (ActiveTriggers >= TriggerToActivate)
	{
		bShouldMoveToEnd = true;
	}
}

void AMovingPlatform::DeactivateInteraction()
{
	ActiveTriggers = FMath::Max(0, ActiveTriggers - 1);

	if (ActiveTriggers < TriggerToActivate)
	{
		bShouldMoveToEnd = false;
	}
}

