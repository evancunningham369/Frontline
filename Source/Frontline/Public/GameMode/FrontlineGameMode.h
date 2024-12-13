// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "FrontlineGameMode.generated.h"

/**
 * 
 */
UCLASS()
class FRONTLINE_API AFrontlineGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	virtual void PlayerEliminated(class AFrontlineCharacter* ElimmedCharacter, class AFrontlinePlayerController* VictimController, AFrontlinePlayerController* AttackerController);
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController);
};
