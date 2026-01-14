// Copyright 2020-2026 NiceBug Games All Rights Reserved.


#include "DoubleDoor.h"

#include "Net/UnrealNetwork.h"


ADoubleDoor::ADoubleDoor()
{
	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates = true;
	SetReplicateMovement(false);
	
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	LeftHinge = CreateDefaultSubobject<USceneComponent>(TEXT("LeftHinge"));
	LeftHinge->SetupAttachment(SceneRoot);

	LeftDoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftDoorMesh"));
	LeftDoorMesh->SetupAttachment(LeftHinge);

	RightHinge = CreateDefaultSubobject<USceneComponent>(TEXT("RightHinge"));
	RightHinge->SetupAttachment(SceneRoot);

	RightDoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightDoorMesh"));
	RightDoorMesh->SetupAttachment(RightHinge);
}

void ADoubleDoor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADoubleDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	float TargetYawLeft  = bIsDoorOpen ? TargetYawAngle : 0.0f;
	float TargetYawRight = bIsDoorOpen ? -TargetYawAngle : 0.0f;
	
	FRotator CurrentLeft  = LeftHinge->GetRelativeRotation();
	FRotator CurrentRight = RightHinge->GetRelativeRotation();
	
	FRotator NewLeft = FMath::RInterpTo(CurrentLeft, FRotator(0, TargetYawLeft, 0), DeltaTime, RotationSpeed);
	FRotator NewRight = FMath::RInterpTo(CurrentRight, FRotator(0, TargetYawRight, 0), DeltaTime, RotationSpeed);
	
	LeftHinge->SetRelativeRotation(NewLeft);
	RightHinge->SetRelativeRotation(NewRight);
}

void ADoubleDoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ADoubleDoor, bIsDoorOpen);
}

void ADoubleDoor::ActivateInteraction()
{
	ActiveTriggers++;
	
	if (ActiveTriggers >= TriggersToOpen)
	{
		bIsDoorOpen = true;
	}
		
}

void ADoubleDoor::DeactivateInteraction()
{
	ActiveTriggers = FMath::Max(0, ActiveTriggers - 1);
	
	if (ActiveTriggers < TriggersToOpen)
	{
		bIsDoorOpen = false;	
	} 
}

