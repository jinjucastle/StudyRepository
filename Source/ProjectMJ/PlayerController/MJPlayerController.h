// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MJPlayerController.generated.h"

class UDataAsset_InputConfig;
/**
 * 
 */
UCLASS()
class PROJECTMJ_API AMJPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMJPlayerController();

protected:

	virtual void SetupInputComponent() override;

	
#pragma region Inputs
	void OnInputStarted();
	void OnSetDestinationTriggered();
	void OnSetDestinationReleased();
	void OnTouchTriggered();
	void OnTouchReleased();
#pragma endregion

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CharacterData", meta= (AllowPrivateAccess="true"))
	UDataAsset_InputConfig* InputConfigDataAsset;

private:

	FVector CachedDestination;

	bool bIsTouch; // Is it a touch device
	float FollowTime; // For how long it has been pressed
};
