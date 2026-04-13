// Copyright Epic Games, Inc. All Rights Reserved.

#include "TP_ThirdPersonCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "TP_ThirdPerson/CustomCharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "MotionWarpingComponent.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ATP_ThirdPersonCharacter

ATP_ThirdPersonCharacter::ATP_ThirdPersonCharacter(const FObjectInitializer& ObjectInitialize)
	:Super(ObjectInitialize.SetDefaultSubobjectClass<UCustomCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	CustomCharactorMovementComponent = Cast<UCustomCharacterMovementComponent>(GetCharacterMovement());
	CustomMotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("CustomMotionWarping"));

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	

}

void ATP_ThirdPersonCharacter::OnPlayerEnterClimb()
{
	CustomAddInputMappingContext(ClimbMappingContext, 1);
}

void ATP_ThirdPersonCharacter::OnPlayerExitClimb()
{
	CustomRemoveInputMappingContext(ClimbMappingContext);
}

void ATP_ThirdPersonCharacter::CustomAddInputMappingContext(UInputMappingContext* MappingContext, int32 InPriority)
{
	if (!MappingContext)return;

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MappingContext, InPriority);
		}
	}
}

void ATP_ThirdPersonCharacter::CustomRemoveInputMappingContext(UInputMappingContext* MappingContext)
{
	if (!MappingContext) return;

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(MappingContext);
		}
	}
}

void ATP_ThirdPersonCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();


	GetCustomCharacterMovement()->OnEnterClimbState.BindUObject(this, &ThisClass::OnPlayerEnterClimb);
	GetCustomCharacterMovement()->OnExitClimbState.BindUObject(this, &ThisClass::OnPlayerExitClimb);
}


//////////////////////////////////////////////////////////////////////////
// Input

void ATP_ThirdPersonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	
	CustomAddInputMappingContext(DefaultMappingContext, 0);
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATP_ThirdPersonCharacter::HandleGroundMovementInput);
		EnhancedInputComponent->BindAction(ClimbMoveAction, ETriggerEvent::Triggered, this, &ATP_ThirdPersonCharacter::HandleClimbMovementInput);
		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATP_ThirdPersonCharacter::Look);

		EnhancedInputComponent->BindAction(TestAction, ETriggerEvent::Started, this, &ATP_ThirdPersonCharacter::Test);

		EnhancedInputComponent->BindAction(ClimbHopAction, ETriggerEvent::Started, this, &ATP_ThirdPersonCharacter::OnClimbHopActionStarted);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ATP_ThirdPersonCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	if (!CustomCharactorMovementComponent) return;

	if (CustomCharactorMovementComponent->IsClimbing())
	{
		HandleClimbMovementInput(Value);
	}
	else
	{
		HandleGroundMovementInput(Value);
	}
}

void ATP_ThirdPersonCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ATP_ThirdPersonCharacter::Test(const FInputActionValue& Value)
{
	//UE_LOG(LogTemp, Error, TEXT("Starting"));
	if (!CustomCharactorMovementComponent)return;
	
	if (!CustomCharactorMovementComponent->IsClimbing())
	{
		CustomCharactorMovementComponent->ToggleClimbing(true);
	}
	else
	{
		CustomCharactorMovementComponent->ToggleClimbing(false);
	}
}

void ATP_ThirdPersonCharacter::HandleGroundMovementInput(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ATP_ThirdPersonCharacter::HandleClimbMovementInput(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	const FVector ForwardDirection = FVector::CrossProduct(
		-CustomCharactorMovementComponent->GetClimbableSurfaceNormal(), GetActorRightVector());
	const FVector RightDirection = FVector::CrossProduct(
		-CustomCharactorMovementComponent->GetClimbableSurfaceNormal(), -GetActorUpVector());

	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void ATP_ThirdPersonCharacter::OnClimbHopActionStarted(const FInputActionValue& Value)
{
	if (CustomCharactorMovementComponent)
	{
		CustomCharactorMovementComponent->RequestHopping();
	}
}
