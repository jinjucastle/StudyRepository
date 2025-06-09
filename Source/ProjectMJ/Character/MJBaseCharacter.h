// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "MJBaseCharacter.generated.h"

class UProjectMJAbilitySystemComponent;
class UProjectMJAttributeSet;

UCLASS()
class PROJECTMJ_API AMJBaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMJBaseCharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;

protected:

	virtual void PossessedBy(AController* NewController) override;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AbilitySystem")
	TObjectPtr<UProjectMJAbilitySystemComponent>ProjectMJAbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	TObjectPtr< UProjectMJAttributeSet>ProjectMJAttributeSet;

public:
	
	FORCEINLINE UProjectMJAbilitySystemComponent* GetProjectMJAbilitySystemComponent() const{ return ProjectMJAbilitySystemComponent; }

	FORCEINLINE UProjectMJAttributeSet* GetProjectMJAttributeSet()const  { return ProjectMJAttributeSet; }
};
