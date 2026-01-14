// Copyright 2020-2026 NiceBug Games All Rights Reserved.


#include "MovableBox.h"

#include "NaniteSceneProxy.h"


AMovableBox::AMovableBox()
{
	PrimaryActorTick.bCanEverTick = false;
	
	bReplicates = true;
	SetReplicateMovement(true);
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("MeshComponent");
	RootComponent = MeshComponent;
	
	MeshComponent->SetSimulatePhysics(true);
	MeshComponent->SetSimulatePhysics(true);
	MeshComponent->SetCollisionProfileName("PhysicsActor");
	MeshComponent->BodyInstance.bLockXRotation = true;
	MeshComponent->BodyInstance.bLockYRotation = true;
	MeshComponent->SetLinearDamping(2.5f); 
	MeshComponent->SetAngularDamping(1.0f);
	
	Tags.Add(FName("Weight"));
}

void AMovableBox::BeginPlay()
{
	Super::BeginPlay();
	
	MeshComponent->SetMassOverrideInKg(NAME_None, 50.0f, true);
}

