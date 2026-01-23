// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CustomCharacterMovementComponent.generated.h"

DECLARE_DELEGATE(FOnEnterClimbState)
DECLARE_DELEGATE(FOnExitClimbState)

class UAnimMontage;
class UAnimInstance;
class ATP_ThirdPersonCharacter;

UENUM(BlueprintType)
namespace ECustomMovementMode
{
	enum Type
	{
		MOVE_Climb UMETA(DisplayName="Climb Mode")
	};
}
/**
 * 
 */
UCLASS()
class CLIMBINGTEST_API UCustomCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
public:
	FOnEnterClimbState OnEnterClimbState;
	FOnExitClimbState OnExitClimbState;

protected: 
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	virtual float GetMaxSpeed() const override;
	virtual float GetMaxAcceleration() const override;
	virtual FVector ConstrainAnimRootMotionVelocity(const FVector& RootMotionVelocity, const FVector& CurrentVelocity) const override;
private:

#pragma region ClimbCore

	bool TraceClimbableSurface();
	FHitResult TraceFromEyeHeight(float TraceDistance, float TraceStartOffset = 0.f, bool bDrawCapsuleLine = false, bool bDrawPersistantShapes = false);
#pragma endregion 

#pragma region ClimbTraces
	TArray<FHitResult> DoCapsuleTraceMultiByObject(const FVector& start, const FVector& end, bool bDrawCapsuleLine = false, bool bDrawPersistantShapes=false);
	FHitResult DoLineTraceSingleByObject(const FVector& start, const FVector& end, bool bDrawCapsuleLine = false, bool bDrawPersistantShapes = false);
#pragma endregion 
	
	TArray<FHitResult>ClimbingTraceHitResult;

	FVector CurrentClimbableSurfaceLocation;

	FVector CurrentClimbableSurfaceNormal;

	UPROPERTY()
	UAnimInstance* OwningPlayerAnimation;

#pragma region ClimbVariables 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing", meta = (AllowPrivateAccess = "true"));
	TArray<TEnumAsByte<EObjectTypeQuery>>ClimbableSurfaceTraceTypes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing", meta = (AllowPrivateAccess = "true"));
	float ClimbCapsuleTraceRadius = 50.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing ", meta = (AllowPrivateAccess = "true"));
	float ClimbCapsuleTraceHalfHeight = 72.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing ", meta = (AllowPrivateAccess = "true"));
	float MaxBreakClimbDeceleration = 400.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing ", meta = (AllowPrivateAccess = "true"));
	float MaxClimbSpeed=100;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing ", meta = (AllowPrivateAccess = "true"));
	float MaxClimbAccelation=300;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing ", meta = (AllowPrivateAccess = "true"));
	float ClimbDownWalkableSurfaceTraceOffset = 200.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing ", meta = (AllowPrivateAccess = "true"));
	float ClimbDownLedgeSurfaceTraceOffset = 50.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing ", meta = (AllowPrivateAccess = "true"));
	ATP_ThirdPersonCharacter* OwningCharacter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing ", meta = (AllowPrivateAccess = "true"));
	UAnimMontage* ClimbMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing ", meta = (AllowPrivateAccess = "true"));
	UAnimMontage* ClimbToTopMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing ", meta = (AllowPrivateAccess = "true"));
	UAnimMontage* ClimbToLedgeMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing ", meta = (AllowPrivateAccess = "true"));
	UAnimMontage* MotionWarpMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing ", meta = (AllowPrivateAccess = "true"));
	UAnimMontage* ClimbHopMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing ", meta = (AllowPrivateAccess = "true"));
	UAnimMontage* ClimbDownHopMontage;


#pragma endregion

public:
	void ToggleClimbing(bool bEnableClimb);
	bool IsClimbing() const;

	bool CanStartClimbing();

	bool CanClimbDownLedge();

	void StartClimbing();
	void StopClimbing();
	void PhysClimb(float deltaTime, int32 Iterations);

	void ProcessClimbableSurfaceInfo();
	bool CheckShouldStopClimbing();

	bool CheckHasReachedFloor();

	bool CheckHasReachedLedge();
	
	bool CheckHasValting(FVector&StartVector,FVector&EndVector);

	void TryValting();

	FQuat GetClimbRotation(float DeltaTime);

	void SnapMovementToClimableSurfaces(float DeltaTime);

	void PlayClimbMontage(UAnimMontage* MontageToPlay);
	
	void SetMotionWarpTarget(const FName& InWarpName, const FVector& InWarpLocation);

	void HandleHopUp();
	bool CheckCanHopUp(FVector& HopVector);

	void HandleHopDown();
	bool CheckCanHopDown(FVector&HopVector);

	UFUNCTION()
	void OnclimbMontageEnded(UAnimMontage* Montage, bool Interrupted);

	FVector GetUnRotatorClimbVelocity()const;

	void RequestHopping();
	
	FORCEINLINE FVector GetClimbableSurfaceNormal()const { return CurrentClimbableSurfaceNormal; }
};
