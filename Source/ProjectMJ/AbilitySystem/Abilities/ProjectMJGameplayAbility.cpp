// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/ProjectMJGameplayAbility.h"
#include "AbilitySystem/ProjectMJAbilitySystemComponent.h"
#include "ProjectMJGameplayAbility.h"

void UProjectMJGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	if (AbilityActivationPolicy == EProjectMJAbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo && !Spec.IsActive()) {
			ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
		}
	}
}

void UProjectMJGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (AbilityActivationPolicy == EProjectMJAbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo)
		{
			ActorInfo->AbilitySystemComponent->ClearAbility(Handle);
		}
	}
}
