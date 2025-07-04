﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ProjectMJGameplayAbility.generated.h"

UENUM(BlueprintType)
enum class EProjectMJAbilityActivationPolicy :uint8
{
	OnTriggered,
	OnGiven
};
/**
 * 
 */
UCLASS()
class PROJECTMJ_API UProjectMJGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
protected:

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)override;

	UPROPERTY(EditDefaultsOnly,Category="Ability")
	EProjectMJAbilityActivationPolicy AbilityActivationPolicy = EProjectMJAbilityActivationPolicy::OnTriggered;
};
