// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MJBaseCharacter.h"
#include "AbilitySystem/ProjectMJAttributeSet.h"
#include "AbilitySystem/ProjectMJAbilitySystemComponent.h"
// Sets default values
AMJBaseCharacter::AMJBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	GetMesh()->bReceivesDecals = false;
	ProjectMJAbilitySystemComponent = CreateDefaultSubobject<UProjectMJAbilitySystemComponent>(TEXT("ProjectMJAbilitySystemComponent"));
	ProjectMJAttributeSet = CreateDefaultSubobject<UProjectMJAttributeSet>(TEXT("ProjectMJAttributeSet"));
}

UAbilitySystemComponent* AMJBaseCharacter::GetAbilitySystemComponent() const
{
	return	GetProjectMJAbilitySystemComponent();
}

void AMJBaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (ProjectMJAbilitySystemComponent)
	{
		ProjectMJAbilitySystemComponent->InitAbilityActorInfo(this, this);
	}

}



