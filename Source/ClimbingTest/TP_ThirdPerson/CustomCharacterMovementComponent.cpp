// Fill out your copyright notice in the Description page of Project Settings.


#include "TP_ThirdPerson/CustomCharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TP_ThirdPersonCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "MotionWarpingComponent.h"




void UCustomCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	OwningPlayerAnimation = CharacterOwner->GetMesh()->GetAnimInstance();

	if (OwningPlayerAnimation)
	{
		OwningPlayerAnimation->OnMontageEnded.AddDynamic(this, &UCustomCharacterMovementComponent::OnclimbMontageEnded);
		OwningPlayerAnimation->OnMontageBlendingOut.AddDynamic(this, &UCustomCharacterMovementComponent::OnclimbMontageEnded);
	}
	OwningCharacter = Cast<ATP_ThirdPersonCharacter>(CharacterOwner);
}

void UCustomCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime,TickType,ThisTickFunction);
	//TraceClimbableSurface();
	//TraceFromEyeHeight(100.f);
	CanClimbDownLedge();
}

void UCustomCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

	if (IsClimbing())
	{
		bOrientRotationToMovement = false;
		CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(48.f);
		OnEnterClimbState.ExecuteIfBound();
	}
	if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == ECustomMovementMode::MOVE_Climb)
	{
		bOrientRotationToMovement = true;
		CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(96.f);
		
		const FRotator DirtyRotation = UpdatedComponent->GetComponentRotation();
		const FRotator CleanStandRotation = FRotator(0.f, DirtyRotation.Yaw, 0.f);
		
		UpdatedComponent->SetRelativeRotation(CleanStandRotation);

		StopMovementImmediately();
		OnExitClimbState.ExecuteIfBound();
	}
}

void UCustomCharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);
	
	if (IsClimbing())
	{
		PhysClimb(deltaTime, Iterations);
	}
}

float UCustomCharacterMovementComponent::GetMaxSpeed() const
{
	if (IsClimbing())
	{
		return MaxClimbSpeed;
	}
	else
	{
		return Super::GetMaxSpeed();
	}
}

float UCustomCharacterMovementComponent::GetMaxAcceleration() const
{
	if (IsClimbing())
	{
		return MaxAcceleration;
	}
	else
	{
		return Super::GetMaxAcceleration();
	}

}

FVector UCustomCharacterMovementComponent::ConstrainAnimRootMotionVelocity(const FVector& RootMotionVelocity, const FVector& CurrentVelocity) const
{
	const bool bIsPlayingRMMontage =
		IsFalling() && OwningPlayerAnimation && OwningPlayerAnimation->IsAnyMontagePlaying();

	if (bIsPlayingRMMontage)
	{
		return RootMotionVelocity;
	}
	else
	{
		return Super::ConstrainAnimRootMotionVelocity(RootMotionVelocity, CurrentVelocity);
	}
}

#pragma region Capsule
TArray<FHitResult> UCustomCharacterMovementComponent::DoCapsuleTraceMultiByObject(const FVector& start, const FVector& end, bool bDrawCapsuleLine, bool bDrawPersistantShapes)
{
	TArray<FHitResult> OutCapsule;

	EDrawDebugTrace::Type DebugTraceType = EDrawDebugTrace::None;

	if (bDrawCapsuleLine)
	{
		DebugTraceType = EDrawDebugTrace::ForOneFrame;
		if (bDrawPersistantShapes)
		{
			DebugTraceType = EDrawDebugTrace::Persistent;
		}
	}

	UKismetSystemLibrary::CapsuleTraceMultiForObjects(
		this,
		start,
		end,
		ClimbCapsuleTraceRadius,
		ClimbCapsuleTraceHalfHeight,
		ClimbableSurfaceTraceTypes,
		false,
		TArray<AActor*>(),
		DebugTraceType,
		OutCapsule,
		false
	);
	return OutCapsule;
}

FHitResult UCustomCharacterMovementComponent::DoLineTraceSingleByObject(const FVector& start, const FVector& end, bool bDrawCapsuleLine, bool bDrawPersistantShapes)
{
	FHitResult OutHit;
	EDrawDebugTrace::Type DebugTraceType = EDrawDebugTrace::None;

	if (bDrawCapsuleLine)
	{
		DebugTraceType = EDrawDebugTrace::ForOneFrame;
		if (bDrawPersistantShapes)
		{
			DebugTraceType = EDrawDebugTrace::Persistent;
		}
	}
	UKismetSystemLibrary::LineTraceSingleForObjects(
		this,
		start,
		end,
		ClimbableSurfaceTraceTypes,
		false,
		TArray<AActor*>(),
		DebugTraceType,
		OutHit,
		false

	);

	return OutHit;

}


#pragma endregion 
#pragma region ClimbCore
//Trace For climbing Surfaces, return true if there are Indeed
bool UCustomCharacterMovementComponent::TraceClimbableSurface()
{
	const FVector StartOffset = UpdatedComponent->GetForwardVector()*30.0f;
	const FVector Start = UpdatedComponent->GetComponentLocation()+StartOffset;
	const FVector End = Start + UpdatedComponent->GetForwardVector();
	ClimbingTraceHitResult=DoCapsuleTraceMultiByObject(Start, End, true);
	return !ClimbingTraceHitResult.IsEmpty();
}
FHitResult UCustomCharacterMovementComponent::TraceFromEyeHeight(float TraceDistance, float TraceStartOffset, bool bDrawCapsuleLine , bool bDrawPersistantShapes)
{
	const FVector ComponentLocation = UpdatedComponent->GetComponentLocation();
	const FVector EyeHeightOffset = UpdatedComponent->GetUpVector() * (CharacterOwner->BaseEyeHeight + TraceStartOffset);

	const FVector Start = ComponentLocation + EyeHeightOffset;
	const FVector End = Start + UpdatedComponent->GetForwardVector() * TraceDistance;

	return DoLineTraceSingleByObject(Start, End, bDrawCapsuleLine, bDrawPersistantShapes);
}

void UCustomCharacterMovementComponent::ToggleClimbing(bool bEnableClimb)
{
	if (bEnableClimb)
	{
		if (CanStartClimbing())
		{
			PlayClimbMontage(ClimbMontage);
		}
		else if(CanClimbDownLedge())
		{
			PlayClimbMontage(ClimbToLedgeMontage);
		}
		else
		{
			TryValting();
		}
	
		
	}
	else
	{
		StopClimbing();
	}
	
}

bool UCustomCharacterMovementComponent::CanStartClimbing()
{
	if (IsFalling()) return false;
	if (!TraceClimbableSurface()) return false;
	if (!TraceFromEyeHeight(100.f).bBlockingHit) return false;
	
	return true;
}

bool UCustomCharacterMovementComponent::CanClimbDownLedge()
{
	if (IsFalling()) return  false;

	const FVector ComponentLocation = UpdatedComponent->GetComponentLocation();
	const FVector ComponentForward = UpdatedComponent->GetForwardVector();
	const FVector DownVector = -UpdatedComponent->GetUpVector();

	const FVector WalkableSurfaceTraceStart = ComponentLocation + ComponentForward * ClimbDownWalkableSurfaceTraceOffset;
	const FVector WalkableSurfaceTraceEnd = WalkableSurfaceTraceStart + DownVector * 100.f;

	FHitResult WalkableSurfaceHit =DoLineTraceSingleByObject(WalkableSurfaceTraceStart, WalkableSurfaceTraceEnd, true);

	const FVector LedgeTraceStart = WalkableSurfaceHit.TraceStart + ComponentForward * ClimbDownLedgeSurfaceTraceOffset;
	const FVector LedgeTraceEnd = LedgeTraceStart + DownVector * 300.0f;

	FHitResult LedgeTraceHit = DoLineTraceSingleByObject(LedgeTraceStart, LedgeTraceEnd, true);

	if (WalkableSurfaceHit.bBlockingHit && !LedgeTraceHit.bBlockingHit)
	{
		return true;
	}

	return false;

}

void UCustomCharacterMovementComponent::StartClimbing()
{
	SetMovementMode(MOVE_Custom, ECustomMovementMode::MOVE_Climb);
}

void UCustomCharacterMovementComponent::StopClimbing()
{
	SetMovementMode(MOVE_Falling);
}

void UCustomCharacterMovementComponent::PhysClimb(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}
	//Process all the Climbable surface info
	TraceClimbableSurface();

	ProcessClimbableSurfaceInfo();
	// Check if we should
	if (CheckShouldStopClimbing()|| CheckHasReachedFloor())
	{
		StopClimbing();
	}
	RestorePreAdditiveRootMotionVelocity();

	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		//Define the max climb speed and Acceleration
		CalcVelocity(deltaTime, 0.f, true, MaxBreakClimbDeceleration);
	}

	ApplyRootMotionToVelocity(deltaTime);

	FVector OldLocation = UpdatedComponent->GetComponentLocation();
	const FVector Adjusted = Velocity * deltaTime;
	FHitResult Hit(1.f);
	//Handle Climb rotation
	SafeMoveUpdatedComponent(Adjusted, GetClimbRotation(deltaTime), true, Hit);

	if (Hit.Time < 1.f)
	{

		HandleImpact(Hit, deltaTime, Adjusted);
		SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);

	}
		if ( !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
		{
			Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / deltaTime;
		}
		SnapMovementToClimableSurfaces(deltaTime);

		if (CheckHasReachedLedge())
		{
			PlayClimbMontage(ClimbToTopMontage);
		}
}

void UCustomCharacterMovementComponent::ProcessClimbableSurfaceInfo()
{
	CurrentClimbableSurfaceLocation = FVector::ZeroVector;
	CurrentClimbableSurfaceNormal = FVector::ZeroVector;

	if (ClimbingTraceHitResult.IsEmpty())return;

	for (const FHitResult& TraceResult : ClimbingTraceHitResult)
	{
		CurrentClimbableSurfaceLocation += TraceResult.ImpactPoint;
		CurrentClimbableSurfaceNormal += TraceResult.ImpactNormal;
	}

	CurrentClimbableSurfaceLocation /= ClimbingTraceHitResult.Num();
	CurrentClimbableSurfaceNormal = CurrentClimbableSurfaceNormal.GetSafeNormal();
	
}

bool UCustomCharacterMovementComponent::CheckShouldStopClimbing()
{
	if (ClimbingTraceHitResult.IsEmpty())return true;
	const float DotResult = FVector::DotProduct(CurrentClimbableSurfaceNormal, FVector::UpVector);
	const float DegreeDiff = FMath::RadiansToDegrees(FMath::Acos(DotResult));
	
	if (DegreeDiff <= 60)
	{
		return true;
	}

	return false;
}

bool UCustomCharacterMovementComponent::CheckHasReachedFloor()
{
	const FVector DownVector = -UpdatedComponent->GetUpVector();
	const FVector StartOffset = DownVector * 50;
	const FVector Start = UpdatedComponent->GetComponentLocation() + StartOffset;
	const FVector End = Start + DownVector;

	TArray<FHitResult> PossibleFloorHits = DoCapsuleTraceMultiByObject(Start, End);
	
	if (PossibleFloorHits.IsEmpty())return false;

	for (const FHitResult& PossibleFloorHit : PossibleFloorHits)
	{
		const bool bFloorReached =
			FVector::Parallel(-PossibleFloorHit.ImpactNormal, FVector::UpVector) &&
			GetUnRotatorClimbVelocity().Z < -10.f;
		
		if (bFloorReached)
		{
			return true;
		}
	}

	return false;
}

bool UCustomCharacterMovementComponent::CheckHasReachedLedge()
{
	FHitResult LedgetHitResult = TraceFromEyeHeight(100.0f, 30.0f);

	if (!LedgetHitResult.bBlockingHit)
	{
		const FVector WalkableSurfaceTraceStart = LedgetHitResult.TraceEnd;
		const FVector DownVector = -UpdatedComponent->GetUpVector();

		const FVector WalkableSurfaceTraceEnd = WalkableSurfaceTraceStart + DownVector * 100.f;
		FHitResult WalkablesutfaceHitResult = DoLineTraceSingleByObject(WalkableSurfaceTraceStart, WalkableSurfaceTraceEnd, true);
		if (WalkablesutfaceHitResult.bBlockingHit && GetUnRotatorClimbVelocity().Z > 10.f  )
		{
			return true;
		}
	}
	return false;
}

void UCustomCharacterMovementComponent::TryValting()
{
	FVector StartVector;
	FVector EndVector;
	if (CheckHasValting(StartVector, EndVector))
	{
		SetMotionWarpTarget("Point1", StartVector);
		SetMotionWarpTarget("Point2", EndVector);

		StartClimbing();
		PlayClimbMontage(MotionWarpMontage);
	}
}
bool UCustomCharacterMovementComponent::CheckHasValting(FVector& StartVector, FVector& EndVector)
{
	if (IsFalling())return false;

	StartVector = FVector::ZeroVector;
	EndVector = FVector::ZeroVector;
	const FVector ComponentLocation = UpdatedComponent->GetRelativeLocation();
	const FVector ComponentForward = UpdatedComponent->GetForwardVector();
	const FVector UpVector = UpdatedComponent->GetUpVector();
	const FVector DownVector = -UpdatedComponent->GetUpVector();

	for (int i = 0; i < 5; i++)
	{
		const FVector Start = ComponentLocation + UpVector * 100 + ComponentForward * 100.0f * (i + 1);
		const FVector End = Start + DownVector * 100.f * (i + 1);
		FHitResult VaultResult = DoLineTraceSingleByObject(Start, End, true, true);
		if (i == 0 && VaultResult.bBlockingHit)
		{
			StartVector = VaultResult.ImpactPoint;
		}
		if (i == 3 && VaultResult.bBlockingHit)
		{
			EndVector = VaultResult.ImpactPoint;
		}
	}

	if (StartVector != FVector::ZeroVector && EndVector != FVector::ZeroVector)
	{
		return true;
	}

	else
	{
		return false;
	}
}


FQuat UCustomCharacterMovementComponent::GetClimbRotation(float DeltaTime)
{
	const FQuat CurrentQuat = UpdatedComponent->GetComponentQuat();
	if (HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity())
	{
		return CurrentQuat;
	}
	const FQuat TargetQuat = FRotationMatrix::MakeFromX(-CurrentClimbableSurfaceNormal).ToQuat();

	return FMath::QInterpTo(CurrentQuat, TargetQuat, DeltaTime, 5.0f);
}

void UCustomCharacterMovementComponent::SnapMovementToClimableSurfaces(float DeltaTime)
{
	const FVector ComponentForward = UpdatedComponent->GetForwardVector();
	const FVector ComponentLocation = UpdatedComponent->GetComponentLocation();

	const FVector ProjectedCharacterToSurface = (CurrentClimbableSurfaceLocation - ComponentLocation).ProjectOnTo(ComponentForward);

	const FVector SnapVector = -CurrentClimbableSurfaceNormal * ProjectedCharacterToSurface.Length();

	UpdatedComponent->MoveComponent(SnapVector * DeltaTime * MaxClimbSpeed,
		UpdatedComponent->GetComponentQuat(),
		true);
}

void UCustomCharacterMovementComponent::PlayClimbMontage(UAnimMontage* MontageToPlay)
{
	if (!MontageToPlay)return;
	if (!OwningPlayerAnimation)return;
	if (OwningPlayerAnimation->IsAnyMontagePlaying())return;

	OwningPlayerAnimation->Montage_Play(MontageToPlay);
}

void UCustomCharacterMovementComponent::SetMotionWarpTarget(const FName& InWarpName, const FVector& InWarpLocation)
{
	if (!OwningCharacter)return;
	OwningCharacter->GetMotionWarpingComponent()->AddOrUpdateWarpTargetFromLocation(InWarpName, InWarpLocation);

}

void UCustomCharacterMovementComponent::HandleHopUp()
{
	FVector HopVector;
	if (CheckCanHopUp(HopVector))
	{
		SetMotionWarpTarget(FName("Super"), HopVector);
		PlayClimbMontage(ClimbHopMontage);
	}
}

bool UCustomCharacterMovementComponent::CheckCanHopUp(FVector& HopVector)
{
	FHitResult HopTarget = TraceFromEyeHeight(100.0f, -20.0f, true);
	FHitResult SaftyLedgeHit = TraceFromEyeHeight(100.f, 150.0f, true);

	if (HopTarget.bBlockingHit && SaftyLedgeHit.bBlockingHit)
	{
		HopVector = HopTarget.ImpactPoint;
		return true;
	}
	
	return false;

}

void UCustomCharacterMovementComponent::HandleHopDown()
{
	FVector HopVector;
	if (CheckCanHopDown(HopVector))
	{
		SetMotionWarpTarget(FName("Down"), HopVector);
		PlayClimbMontage(ClimbDownHopMontage);
	}
}

bool UCustomCharacterMovementComponent::CheckCanHopDown(FVector& HopVector)
{
	
	FHitResult SaftyLegdeHit = TraceFromEyeHeight(100.0f, -280.0f, true);

	if (SaftyLegdeHit.bBlockingHit)
	{
		HopVector = SaftyLegdeHit.ImpactPoint;
		return true;
	}

	return false;
}

bool UCustomCharacterMovementComponent::IsClimbing() const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == ECustomMovementMode::MOVE_Climb;

}

void UCustomCharacterMovementComponent::OnclimbMontageEnded(UAnimMontage* Montage, bool Interrupted)
{
	if (Montage == ClimbMontage||Montage==ClimbToLedgeMontage||Montage==ClimbHopMontage || Montage == ClimbDownHopMontage)
	{
		StartClimbing();
		StopMovementImmediately();
	}
	else
	{
		SetMovementMode(MOVE_Walking);
	}

}

FVector UCustomCharacterMovementComponent::GetUnRotatorClimbVelocity() const
{
	return UKismetMathLibrary::Quat_UnrotateVector(UpdatedComponent->GetComponentQuat(), Velocity);
}

void UCustomCharacterMovementComponent::RequestHopping()
{
	const FVector UnrotatedLastInputVector =
		UKismetMathLibrary::Quat_UnrotateVector(UpdatedComponent->GetComponentQuat(), GetLastInputVector());
	const float DotResult =
		FVector::DotProduct(UnrotatedLastInputVector, FVector::UpVector);

	if (DotResult >= 0.9f)
	{
		HandleHopUp();
	}
	else if (DotResult <= -0.9f)
	{
		HandleHopDown();
	}
	else
	{

	}
}

#pragma endregion