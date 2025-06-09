// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/MJPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "DataAsset/DataAsset_InputConfig.h"
#include "Components/Input/ProjectMJEnhancedInputComponent.h"
#include "ProjectMJGamePlayTags.h"

AMJPlayerController::AMJPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
}

void AMJPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InputConfigDataAsset->DefaultMappingContext, 0);

	}
	UProjectMJEnhancedInputComponent* ProjectMJInputComponent = CastChecked< UProjectMJEnhancedInputComponent>(InputComponent);
	
	//Setup mouse input events
	ProjectMJInputComponent->BindNativeInputAction(InputConfigDataAsset, ProjectMJGameplayTags::InputTag_SetDestination_Click, ETriggerEvent::Started, this, &AMJPlayerController::OnInputStarted);
	ProjectMJInputComponent->BindNativeInputAction(InputConfigDataAsset, ProjectMJGameplayTags::InputTag_SetDestination_Click, ETriggerEvent::Triggered, this, &AMJPlayerController::OnSetDestinationTriggered);
	ProjectMJInputComponent->BindNativeInputAction(InputConfigDataAsset, ProjectMJGameplayTags::InputTag_SetDestination_Click, ETriggerEvent::Completed, this, &AMJPlayerController::OnSetDestinationReleased);
	ProjectMJInputComponent->BindNativeInputAction(InputConfigDataAsset, ProjectMJGameplayTags::InputTag_SetDestination_Click, ETriggerEvent::Canceled, this, &AMJPlayerController::OnSetDestinationReleased);

	ProjectMJInputComponent->BindNativeInputAction(InputConfigDataAsset, ProjectMJGameplayTags::InputTag_SetDestination_Touch, ETriggerEvent::Started, this, &AMJPlayerController::OnInputStarted);
	ProjectMJInputComponent->BindNativeInputAction(InputConfigDataAsset, ProjectMJGameplayTags::InputTag_SetDestination_Touch, ETriggerEvent::Triggered, this, &AMJPlayerController::OnTouchTriggered);
	ProjectMJInputComponent->BindNativeInputAction(InputConfigDataAsset, ProjectMJGameplayTags::InputTag_SetDestination_Touch, ETriggerEvent::Completed, this, &AMJPlayerController::OnTouchReleased);
	ProjectMJInputComponent->BindNativeInputAction(InputConfigDataAsset, ProjectMJGameplayTags::InputTag_SetDestination_Touch, ETriggerEvent::Canceled, this, &AMJPlayerController::OnTouchReleased);

}

void AMJPlayerController::OnInputStarted()
{
	StopMovement();
}

void AMJPlayerController::OnSetDestinationTriggered()
{
	// We flag that the input is being pressed
	FollowTime += GetWorld()->GetDeltaSeconds();

	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	bool bHitSuccessful = false;
	if (bIsTouch)
	{
		bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, true, Hit);
	}
	else
	{
		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	}

	// If we hit a surface, cache the location
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}

	// Move towards mouse pointer or touch
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
	}
}

void AMJPlayerController::OnSetDestinationReleased()
{
	FollowTime = 0.f;
}

void AMJPlayerController::OnTouchTriggered()
{
	bIsTouch = true;
	OnSetDestinationTriggered();
}

void AMJPlayerController::OnTouchReleased()
{
	bIsTouch = false;
	OnSetDestinationReleased();
}
