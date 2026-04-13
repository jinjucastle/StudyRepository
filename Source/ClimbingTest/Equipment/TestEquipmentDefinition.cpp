// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/TestEquipmentDefinition.h"
#include "TestEquipmentInstance.h"


UTestEquipmentDefinition::UTestEquipmentDefinition(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	InstanceType = UTestEquipmentInstance::StaticClass();
}


