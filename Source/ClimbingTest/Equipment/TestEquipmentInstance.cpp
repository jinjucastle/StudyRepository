// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/TestEquipmentInstance.h"
#include "TestEquipmentInstance.h"

UTestEquipmentInstance::UTestEquipmentInstance(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{

}

APawn* UTestEquipmentInstance::GetPawn()const
{

	return Cast<APawn>(GetOuter());
}

APawn* UTestEquipmentInstance::GetTypePawn(TSubclassOf<APawn> PawnType) const
{
	APawn* Result = nullptr;
	if (UClass* ActualpawnType = PawnType)
	{
		if (GetOuter()->IsA(ActualpawnType))
		{
			Result = Cast<APawn>(GetOuter());
		}
	}
	return Result;
}

void UTestEquipmentInstance::OnTestEquipped()
{

}

void UTestEquipmentInstance::OnTestUnequipped()
{

}

void UTestEquipmentInstance::SpawnEquipmentActor()
{
}

void UTestEquipmentInstance::DestoryEquipmentActor()
{
}
