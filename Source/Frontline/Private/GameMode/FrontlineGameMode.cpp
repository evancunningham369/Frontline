// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/FrontlineGameMode.h"
#include "Characters/FrontlineCharacter.h"
#include "PlayerController/FrontlinePlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "PlayerState/FrontlinePlayerState.h"
#include "GameState/FrontlineGameState.h"

namespace MatchState
{
	const FName Cooldown = FName("Cooldown");
}

AFrontlineGameMode::AFrontlineGameMode()
{
	bDelayedStart = true;
}

void AFrontlineGameMode::BeginPlay()
{
	Super::BeginPlay();

	LevelStartingTime = GetWorld()->GetTimeSeconds();
}

void AFrontlineGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			StartMatch();
		}
	}
	else if (MatchState == MatchState::InProgress)
	{
		CountdownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			SetMatchState(MatchState::Cooldown);
		}
	}
	else if (MatchState == MatchState::Cooldown)
	{
		CountdownTime = CooldownTime + WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			RestartGame();
		}
	}
}

void AFrontlineGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AFrontlinePlayerController* FrontlinePlayer = Cast<AFrontlinePlayerController>(*It);
		if (FrontlinePlayer)
		{
			FrontlinePlayer->OnMatchStateSet(MatchState);
		}
	}
}

void AFrontlineGameMode::PlayerEliminated(AFrontlineCharacter* ElimmedCharacter, AFrontlinePlayerController* VictimController, AFrontlinePlayerController* AttackerController)
{
	AFrontlinePlayerState* AttackerPlayerState = AttackerController ? Cast<AFrontlinePlayerState>(AttackerController->PlayerState) : nullptr;
	AFrontlinePlayerState* VictimPlayerState = VictimController ? Cast<AFrontlinePlayerState>(VictimController->PlayerState) : nullptr;

	AFrontlineGameState* FrontlineGameState = GetGameState<AFrontlineGameState>();

	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState && FrontlineGameState)
	{
		AttackerPlayerState->AddToScore(1.f);
		FrontlineGameState->UpdateTopScore(AttackerPlayerState);
	}
	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
	}
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim();
	}
}

void AFrontlineGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}
	if (ElimmedController)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
	}
}