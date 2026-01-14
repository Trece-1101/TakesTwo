// Copyright Epic Games, Inc. All Rights Reserved.

#include "TakesTwoCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "TakesTwo.h"
#include "Interactables/PickUpKey.h"
#include "Net/UnrealNetwork.h"

void ATakesTwoCharacter::OnRep_SkinIndex()
{
	if (AvailableSkins.IsValidIndex(SkinIndex))
	{
		const FSkinInfo& SelectedSkin = AvailableSkins[SkinIndex];

		if (SelectedSkin.Mesh)
		{
			GetMesh()->SetSkeletalMesh(SelectedSkin.Mesh);
		}

		if (SelectedSkin.AnimClass)
		{
			GetMesh()->SetAnimInstanceClass(SelectedSkin.AnimClass);
		}
	}
}

ATakesTwoCharacter::ATakesTwoCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ATakesTwoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATakesTwoCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ATakesTwoCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATakesTwoCharacter::Look);
		
		// Play Emote
		EnhancedInputComponent->BindAction(PlayEmoteAction, ETriggerEvent::Started, this, &ATakesTwoCharacter::PlayEmote);
	}
	else
	{
		UE_LOG(LogTakesTwo, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ATakesTwoCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void ATakesTwoCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void ATakesTwoCharacter::PlayEmote()
{
	if (CanPlayEmote())
	{
		Server_PlayEmote();
	}
}

void ATakesTwoCharacter::Server_PlayEmote_Implementation()
{
	if (CanPlayEmote())
	{
		Multicast_PlayEmote();
	}
}

void ATakesTwoCharacter::Multicast_PlayEmote_Implementation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	
	if (AnimInstance && EmoteMontage)
	{
		if (GetCharacterMovement())
		{
			GetCharacterMovement()->DisableMovement();
		}

		AnimInstance->Montage_Play(EmoteMontage);

		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &ATakesTwoCharacter::OnEmoteEnded);
		AnimInstance->Montage_SetEndDelegate(EndDelegate, EmoteMontage);
	}
}

void ATakesTwoCharacter::Server_PickUpActor_Implementation(AActor* ItemToPickup)
{
	if (HeldItem) { return; }
	if (!ItemToPickup) { return; }

	if (APickUpKey* Key = Cast<APickUpKey>(ItemToPickup))
	{
		HeldItem = Key;

		Key->OnPickedUp();
		
		FAttachmentTransformRules AttachmentRules(
			EAttachmentRule::SnapToTarget, 
			EAttachmentRule::SnapToTarget, 
			EAttachmentRule::KeepWorld, 
			false
		);
		
		Key->AttachToComponent(
			GetMesh(), 
			AttachmentRules, 
			FName("RightHandSocket") 
		);
	}
}

void ATakesTwoCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void ATakesTwoCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void ATakesTwoCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void ATakesTwoCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

void ATakesTwoCharacter::SetSkinIndex(int32 NewIndex)
{	
	if (HasAuthority())
	{
		SkinIndex = NewIndex;
		OnRep_SkinIndex();
	}
}

void ATakesTwoCharacter::TryPickupItem(AActor* ItemToPickup)
{
	if (HeldItem) { return; }
	
	Server_PickUpActor(ItemToPickup);
}

void ATakesTwoCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ATakesTwoCharacter, SkinIndex);
	DOREPLIFETIME(ATakesTwoCharacter, HeldItem);
}

bool ATakesTwoCharacter::CanPlayEmote() const
{
	if (!EmoteMontage) { return false; }
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance) { return false; }
	
	if (AnimInstance->Montage_IsPlaying(EmoteMontage)) { return false; }
	
	if (GetCharacterMovement() && GetCharacterMovement()->IsFalling())
	{
		return false;
	}
	
	return true;
}

void ATakesTwoCharacter::OnEmoteEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}
