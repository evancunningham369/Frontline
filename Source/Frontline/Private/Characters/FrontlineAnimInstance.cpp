// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/FrontlineAnimInstance.h"
#include "Characters/FrontlineCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapon/Weapon.h"

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
		bWeaponEquipped = FrontlineCharacter->IsWeaponEquipped();
		EquippedWeapon = FrontlineCharacter->GetEquippedWeapon();
		bIsCrouched = FrontlineCharacter->bIsCrouched;
		bAiming = FrontlineCharacter->IsAiming();
		TurningInPlace = FrontlineCharacter->GetTurningInPlace();

		// Offset yaw for strafing
		FRotator AimRotation = FrontlineCharacter->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(FrontlineCharacter->GetVelocity());
		FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
		DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 6.f);
		YawOffset = DeltaRotation.Yaw;
		
		// Caluclate leaning
		CharacterRotationLastFrame = CharacterRotation;
		CharacterRotation = FrontlineCharacter->GetActorRotation();
		const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
		const float Target = Delta.Yaw / DeltaTime;
		const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.f);
		Lean = FMath::Clamp(Interp, -90.f, 90.f);

		AO_Yaw = FrontlineCharacter->GetAO_Yaw();
		AO_Pitch = FrontlineCharacter->GetAO_Pitch();

		if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && FrontlineCharacter->GetMesh())
		{
			LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
			FVector OutPosition;
			FRotator OutRotation;
			FrontlineCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
			LeftHandTransform.SetLocation(OutPosition);
			LeftHandTransform.SetRotation(FQuat(OutRotation));

			if (FrontlineCharacter->IsLocallyControlled())
			{
				bLocallyControlled = true;
				FTransform RightHandTransform = FrontlineCharacter->GetMesh()->GetSocketTransform(FName("Hand_R"), ERelativeTransformSpace::RTS_World);
				RightHandRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - FrontlineCharacter->GetHitTarget()) );
			}

			/*FTransform MuzzleTipTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("MuzzleFlash"), ERelativeTransformSpace::RTS_World);
			FVector MuzzleX(FRotationMatrix(MuzzleTipTransform.GetRotation().Rotator()).GetUnitAxis(EAxis::X));
			DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), MuzzleTipTransform.GetLocation() + MuzzleX * 1000.f, FColor::Red);
			DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), FrontlineCharacter->GetHitTarget(), FColor::Orange);*/
		}
	}
}
