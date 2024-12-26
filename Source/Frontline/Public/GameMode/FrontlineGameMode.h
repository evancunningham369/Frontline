// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "FrontlineGameMode.generated.h"

namespace MatchState
{
	extern FRONTLINE_API const FName Cooldown; // Match duration has been reached. Display winner and begin Cooldown timer
}

/**
 * 
 */
UCLASS()
class FRONTLINE_API AFrontlineGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	AFrontlineGameMode();
	virtual void Tick(float DeltaTime) override;
	virtual void PlayerEliminated(class AFrontlineCharacter* ElimmedCharacter, class AFrontlinePlayerController* VictimController, AFrontlinePlayerController* AttackerController);
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController);


	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 10.f;
	
	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 120.f;
	
	UPROPERTY(EditDefaultsOnly)
	float CooldownTime = 10.f;

	float LevelStartingTime = 0.f;

protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;
private:
	float CountdownTime = 0.f;
public:
	FORCEINLINE float GetCountdownTime() { return CountdownTime; }
};
