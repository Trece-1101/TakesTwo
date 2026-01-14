// Copyright 2020-2026 NiceBug Games All Rights Reserved.


#include "PickUpKey.h"

#include "TakesTwoCharacter.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"


APickUpKey::APickUpKey()
{
	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates = true;
	SetReplicateMovement(true);
	
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("MeshComponent");
	MeshComponent->SetupAttachment(SceneRoot);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetSimulatePhysics(false);
	
	TriggerSphere = CreateDefaultSubobject<USphereComponent>("TriggerSphere");
	TriggerSphere->SetupAttachment(SceneRoot);
	TriggerSphere->SetCollisionProfileName(TEXT("Trigger"));
}

void APickUpKey::BeginPlay()
{
	Super::BeginPlay();
	
	InitialLocation = GetActorLocation();
}

void APickUpKey::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	if (TriggerSphere)
	{		
		TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &APickUpKey::OnOverlapBegin);
	}	
}

void APickUpKey::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bIsPickedUp) { return; }

	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += DeltaTime * RotationSpeed;
	SetActorRotation(NewRotation);
	
	FVector NewLocation = InitialLocation;
	float Time = GetGameTimeSinceCreation();
	NewLocation.Z += FMath::Sin(Time * BobbingSpeed) * BobbingHeight;
	SetActorLocation(NewLocation);
}

void APickUpKey::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(APickUpKey, bIsPickedUp);
}

void APickUpKey::OnPickedUp()
{
	bIsPickedUp = true;
	OnRep_IsPickedUp();	
}

void APickUpKey::OnRep_IsPickedUp()
{
	if (bIsPickedUp)
	{
		if (TriggerSphere)
		{
			TriggerSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}			

		SetActorRotation(FRotator::ZeroRotator);
	}
}

void APickUpKey::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bIsPickedUp) { return; }

	if (ATakesTwoCharacter* Character = Cast<ATakesTwoCharacter>(OtherActor))
	{
		Character->TryPickupItem(this);
	}
}

