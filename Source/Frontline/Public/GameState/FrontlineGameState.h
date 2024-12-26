// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "FrontlineGameState.generated.h"

/**
 * 
 */
UCLASS()
class FRONTLINE_API AFrontlineGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void UpdateTopScore(class AFrontlinePlayerState* ScoringPlayer);


	UPROPERTY(Replicated)
	TArray<AFrontlinePlayerState*> TopScoringPlayers;
private:
	float TopScore = 0.f;
};
