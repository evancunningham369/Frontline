// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/FrontlineCharacter.h"

AFrontlineCharacter::AFrontlineCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AFrontlineCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFrontlineCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFrontlineCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

