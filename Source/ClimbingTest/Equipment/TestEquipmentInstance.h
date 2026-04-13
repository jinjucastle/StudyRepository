// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TestEquipmentInstance.generated.h"

class AActor;
class APawn;
/**
 * 
 */
UCLASS(BlueprintType,Blueprintable)
class CLIMBINGTEST_API UTestEquipmentInstance : public UObject
{
	GENERATED_BODY()

public:

	UTestEquipmentInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	
	UFUNCTION(BlueprintPure,Category=Equipment)
	APawn* GetPawn()const;

	UFUNCTION(BlueprintPure,Category=Equipment)
	AActor* GetSpawnedActor()const { return SpawnActors; }

	UFUNCTION(BlueprintPure, Category = Equipment, meta = (DeterminesOutputType = PanwType))
	APawn* GetTypePawn(TSubclassOf<APawn>PawnType)const;

	virtual void OnTestEquipped();
	virtual void OnTestUnequipped();
	virtual void SpawnEquipmentActor();
	virtual void DestoryEquipmentActor();


private:
	
	UPROPERTY()
	TObjectPtr<AActor> SpawnActors;
};
