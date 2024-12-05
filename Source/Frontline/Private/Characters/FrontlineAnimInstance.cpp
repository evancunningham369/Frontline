// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/FrontlineAnimInstance.h"
#include "Characters/FrontlineCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UFrontlineAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	FrontlineCharacter = Cast<AFrontlineCharacter>(TryGetPawnOwner());
	if (FrontlineCharacter)
	{
		FrontlineCharacterMovement = FrontlineCharacter->GetCharacterMovement();
	}
}

void UFrontlineAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (FrontlineCharacterMovement)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(FrontlineCharacterMovement->Velocity);
		IsFalling = FrontlineCharacterMovement->IsFalling();
		bIsAccelerating = FrontlineCharacterMovement->GetCurrentAcceleration().Size() > 0.f ? true : false;
	}
}
