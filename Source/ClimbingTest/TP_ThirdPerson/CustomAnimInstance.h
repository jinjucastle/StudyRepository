// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CustomAnimInstance.generated.h"

class ATP_ThirdPersonCharacter;
class UCustomCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class CLIMBINGTEST_API UCustomAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
protected:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY()
	ATP_ThirdPersonCharacter* CustomCharacter;

	UPROPERTY()
	UCustomCharacterMovementComponent* CustomCharacterMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing", meta = (AllowPrivateAccess = "true"));
	float GroundSpeed;
	void GetGroundSpeed();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing", meta = (AllowPrivateAccess = "true"));
	float AirSpeed;
	void GetAirSpeed();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing", meta = (AllowPrivateAccess = "true"));
	bool bIsMove;
	void GetShouldMove();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing", meta = (AllowPrivateAccess = "true"));
	bool bIsFalling;
	void GetIsFalling();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing", meta = (AllowPrivateAccess = "true"));
	bool bIsClimbing;
	void GetIsClimbing();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing", meta = (AllowPrivateAccess = "true"));
	FVector AnimVelocity;
	void GetAnimVelocity();
	
};
