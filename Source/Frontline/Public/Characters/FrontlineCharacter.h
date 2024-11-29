// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FrontlineCharacter.generated.h"

UCLASS()
class FRONTLINE_API AFrontlineCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AFrontlineCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
