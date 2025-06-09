// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/MJBaseCharacter.h"
#include "MJPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
/**
 * 
 */
UCLASS()
class PROJECTMJ_API AMJPlayerCharacter : public AMJBaseCharacter
{
	GENERATED_BODY()

public:
	AMJPlayerCharacter();

protected: 
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;
private:

#pragma region Components

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess=true))
	TObjectPtr<USpringArmComponent> CameraBoom;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	TObjectPtr<UCameraComponent> FollowCamera;
#pragma endregion 
	
};
