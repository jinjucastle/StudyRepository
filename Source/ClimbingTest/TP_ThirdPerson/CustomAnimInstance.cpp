// Fill out your copyright notice in the Description page of Project Settings.


#include "TP_ThirdPerson/CustomAnimInstance.h"
#include "CustomAnimInstance.h"
#include "TP_ThirdPersonCharacter.h"
#include "CustomCharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UCustomAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	CustomCharacter = Cast<ATP_ThirdPersonCharacter>(TryGetPawnOwner());
	if (CustomCharacter)
	{
		CustomCharacterMovement = CustomCharacter->GetCustomCharacterMovement();
	}
}

void UCustomAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!CustomCharacter || !CustomCharacterMovement) return;

	GetGroundSpeed();
	GetAirSpeed();
	GetShouldMove();
	GetIsFalling();
	GetIsClimbing();
	GetAnimVelocity();

	const FTestCharacterGroundInfo& GroundInfo = CustomCharacterMovement->GetGroundInfo();
	GroundDistance = GroundInfo.GroundDistance;
}

void UCustomAnimInstance::GetGroundSpeed()
{
	GroundSpeed = UKismetMathLibrary::VSizeXY(CustomCharacter->GetVelocity());
}

void UCustomAnimInstance::GetAirSpeed()
{
	AirSpeed = CustomCharacter->GetVelocity().Z;
}

void UCustomAnimInstance::GetShouldMove()
{
	bIsMove=CustomCharacterMovement->GetCurrentAcceleration().Size() > 0 && GroundSpeed > 5.0f && !bIsFalling;
}

void UCustomAnimInstance::GetIsFalling()
{
	bIsFalling = CustomCharacterMovement->IsFalling();
}

void UCustomAnimInstance::GetIsClimbing()
{
	bIsClimbing = CustomCharacterMovement->IsClimbing();
}

void UCustomAnimInstance::GetAnimVelocity()
{
	AnimVelocity = CustomCharacterMovement->GetUnRotatorClimbVelocity();
}
