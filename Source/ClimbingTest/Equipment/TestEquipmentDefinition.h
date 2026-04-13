// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "TestEquipmentDefinition.generated.h"
/**
 * 
 */
class AActor;
class UTestEquipmentInstance;


USTRUCT()
struct FTestEquipmentActorToSpawn
{
	GENERATED_BODY()

	FTestEquipmentActorToSpawn(){}

	UPROPERTY(EditAnywhere, Category=Equipment)
	TSubclassOf<AActor> ActortoSpawn;

	UPROPERTY(EditAnywhere, Category=Equipment)
	FName AttachSocket;
	
	UPROPERTY(EditAnywhere, Category = Equipment)
	FTransform AttacjTransform;
};

UCLASS()
class CLIMBINGTEST_API UTestEquipmentDefinition: public UObject
{

	GENERATED_BODY()

public:
	
	UTestEquipmentDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TSubclassOf<UTestEquipmentInstance>InstanceType;

	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	FTestEquipmentActorToSpawn ActorToSpawn;
	
};
