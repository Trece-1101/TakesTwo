// Copyright 2020-2026 NiceBug Games All Rights Reserved.


#include "Door.h"

#include "TakesTwo.h"
#include "Net/UnrealNetwork.h"


ADoor::ADoor()
{
	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates = true;
	SetReplicateMovement(false);
	
	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	RootComponent = SceneRoot;
	
	HingeComp = CreateDefaultSubobject<USceneComponent>("HingeComponent");
	HingeComp->SetupAttachment(SceneRoot);
	
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>("DoorMesh");
	DoorMesh->SetupAttachment(HingeComp);
}

void ADoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ADoor, bIsDoorOpen);
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();
	
	ClosedRotation = HingeComp->GetRelativeRotation();
	OpenRotation = ClosedRotation + FRotator(0.0f, TargetYawAngle, 0.0f);
}

void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	FRotator CurrentRot = HingeComp->GetRelativeRotation();
	FRotator TargetRot = bIsDoorOpen ? OpenRotation : ClosedRotation;

	if (CurrentRot.Equals(TargetRot, 0.1f)) return;

	FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, RotationSpeed);
    
	HingeComp->SetRelativeRotation(NewRot);
}

void ADoor::ActivateInteraction()
{
	ActiveTriggers++;
	
	if (ActiveTriggers >= TriggersToOpen)
	{
		bIsDoorOpen = true;
	}
}

void ADoor::DeactivateInteraction()
{
	ActiveTriggers = FMath::Max(0, ActiveTriggers - 1);
	if (ActiveTriggers < TriggersToOpen)
	{
		bIsDoorOpen = false;
	}
}

