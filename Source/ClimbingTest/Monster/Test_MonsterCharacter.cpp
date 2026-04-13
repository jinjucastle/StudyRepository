// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Test_MonsterCharacter.h"

// Sets default values
ATest_MonsterCharacter::ATest_MonsterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void ATest_MonsterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATest_MonsterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATest_MonsterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

