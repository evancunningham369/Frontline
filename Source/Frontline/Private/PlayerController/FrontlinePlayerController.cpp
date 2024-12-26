// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/FrontlinePlayerController.h"
#include "HUD/FrontlineHUD.h"
#include "HUD/CharacterOverlay.h"
#include "HUD/Announcement.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Characters/FrontlineCharacter.h"
#include "Net/UnrealNetwork.h"
#include "GameMode/FrontlineGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "FrontlineComponents/CombatComponent.h"
#include "GameState/FrontlineGameState.h"
#include "PlayerState/FrontlinePlayerState.h"


void AFrontlinePlayerController::BeginPlay()
{
	Super::BeginPlay();

	FrontlineHUD = Cast<AFrontlineHUD>(GetHUD());
	ServerCheckMatchState();
}

void AFrontlinePlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetHUDTime();

	CheckTimeSync(DeltaTime);
}

void AFrontlinePlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFrontlinePlayerController, MatchState);
}

void AFrontlinePlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
}

void AFrontlinePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AFrontlineCharacter* FrontlineCharacter = Cast<AFrontlineCharacter>(InPawn);
	if (FrontlineCharacter)
	{
		SetHUDHealth(FrontlineCharacter->GetHealth(), FrontlineCharacter->GetMaxHealth());
	}
}


void AFrontlinePlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	FrontlineHUD = FrontlineHUD == nullptr ? Cast<AFrontlineHUD>(GetHUD()) : FrontlineHUD;
	bool bHUDValid = FrontlineHUD && 
		FrontlineHUD->CharacterOverlay &&
		FrontlineHUD->CharacterOverlay->HealthBar && 
		FrontlineHUD->CharacterOverlay->HealthText;

	if (bHUDValid)
	{
		const float HealthPercent = Health / MaxHealth;
		FrontlineHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		FrontlineHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
}

void AFrontlinePlayerController::SetHUDScore(float Score)
{
	FrontlineHUD = FrontlineHUD == nullptr ? Cast<AFrontlineHUD>(GetHUD()) : FrontlineHUD;
	bool bHUDValid = FrontlineHUD &&
		FrontlineHUD->CharacterOverlay &&
		FrontlineHUD->CharacterOverlay->ScoreAmount;

	if (bHUDValid)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));
		FrontlineHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));
	}
}

void AFrontlinePlayerController::SetHUDDefeats(int32 Defeats)
{
	FrontlineHUD = FrontlineHUD == nullptr ? Cast<AFrontlineHUD>(GetHUD()) : FrontlineHUD;
	bool bHUDValid = FrontlineHUD &&
		FrontlineHUD->CharacterOverlay &&
		FrontlineHUD->CharacterOverlay->DefeatsAmount;

	if (bHUDValid)
	{
		FString DefeatsText = FString::Printf(TEXT("%d"), Defeats);
		FrontlineHUD->CharacterOverlay->DefeatsAmount->SetText(FText::FromString(DefeatsText));
	}
}

void AFrontlinePlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	FrontlineHUD = FrontlineHUD == nullptr ? Cast<AFrontlineHUD>(GetHUD()) : FrontlineHUD;
	bool bHUDValid = FrontlineHUD &&
		FrontlineHUD->CharacterOverlay &&
		FrontlineHUD->CharacterOverlay->WeaponAmmoAmount;

	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		FrontlineHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

void AFrontlinePlayerController::SetHUDCarriedAmmo(int32 Ammo)
{
	FrontlineHUD = FrontlineHUD == nullptr ? Cast<AFrontlineHUD>(GetHUD()) : FrontlineHUD;
	bool bHUDValid = FrontlineHUD &&
		FrontlineHUD->CharacterOverlay &&
		FrontlineHUD->CharacterOverlay->CarriedAmmoAmount;

	if (bHUDValid)
	{
		FString CarriedAmmoText = FString::Printf(TEXT("%d"), Ammo);
		FrontlineHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(CarriedAmmoText));
	}
}

void AFrontlinePlayerController::SetHUDMatchCountdown(float CountdownTime)
{
	FrontlineHUD = FrontlineHUD == nullptr ? Cast<AFrontlineHUD>(GetHUD()) : FrontlineHUD;
	bool bHUDValid = FrontlineHUD &&
		FrontlineHUD->CharacterOverlay &&
		FrontlineHUD->CharacterOverlay->MatchCountdownText;

	if (bHUDValid)
	{
		if (CountdownTime < 0.f)
		{
			FrontlineHUD->CharacterOverlay->MatchCountdownText->SetText(FText());
			return;
		}

		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;

		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		FrontlineHUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownText));
	}
}

void AFrontlinePlayerController::SetHUDAnnouncementCountdown(float CountdownTime)
{
	FrontlineHUD = FrontlineHUD == nullptr ? Cast<AFrontlineHUD>(GetHUD()) : FrontlineHUD;
	bool bHUDValid = FrontlineHUD &&
		FrontlineHUD->Announcement &&
		FrontlineHUD->Announcement->WarmupTime;

	if (bHUDValid)
	{
		if (CountdownTime < 0.f)
		{
			FrontlineHUD->Announcement->WarmupTime->SetText(FText());
			return;
		}

		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;

		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		FrontlineHUD->Announcement->WarmupTime->SetText(FText::FromString(CountdownText));
	}
}

void AFrontlinePlayerController::SetHUDTime()
{
	float TimeLeft = 0.f;
	if (MatchState == MatchState::WaitingToStart) TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::InProgress) TimeLeft = WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::Cooldown) TimeLeft = TimeLeft = CooldownTime + WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;

	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);

	if (HasAuthority())
	{
		FrontlineGameMode = FrontlineGameMode == nullptr ? Cast<AFrontlineGameMode>(UGameplayStatics::GetGameMode(this)) : FrontlineGameMode;
		if (FrontlineGameMode)
		{
			SecondsLeft = FMath::CeilToInt(FrontlineGameMode->GetCountdownTime() + LevelStartingTime);
		}
	}

	if (CountdownInt != SecondsLeft)
	{
		if (MatchState == MatchState::WaitingToStart || MatchState == MatchState::Cooldown)
		{
			SetHUDAnnouncementCountdown(TimeLeft);
		}
		if (MatchState == MatchState::InProgress)
		{
			SetHUDMatchCountdown(TimeLeft);
		}
	}

	CountdownInt = SecondsLeft;
}

float AFrontlinePlayerController::GetServerTime()
{
	if (HasAuthority()) return GetWorld()->GetTimeSeconds();
	else return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void AFrontlinePlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}


void AFrontlinePlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void AFrontlinePlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest, float TimeServerReceivedClientRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	float CurrentServerTime = TimeServerReceivedClientRequest + (0.5f * RoundTripTime);
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

void AFrontlinePlayerController::OnMatchStateSet(FName State)
{
	MatchState = State;

	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}


void AFrontlinePlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void AFrontlinePlayerController::HandleMatchHasStarted()
{
	FrontlineHUD = FrontlineHUD == nullptr ? Cast<AFrontlineHUD>(GetHUD()) : FrontlineHUD;
	if (FrontlineHUD)
	{
		if (FrontlineHUD->Announcement)
		{
			FrontlineHUD->SetAnnouncementOverlayVisibility(false);
		}
		FrontlineHUD->SetCharacterOverlayVisibility(true);
	}
}

void AFrontlinePlayerController::HandleCooldown()
{
	FrontlineHUD = FrontlineHUD == nullptr ? Cast<AFrontlineHUD>(GetHUD()) : FrontlineHUD;
	if (FrontlineHUD)
	{
		bool bHUDValid = FrontlineHUD->Announcement && 
			FrontlineHUD->Announcement->AnnouncementText && 
			FrontlineHUD->Announcement->InfoText;

		if (bHUDValid)
		{
			FrontlineHUD->SetAnnouncementOverlayVisibility(true);
			FString AnnouncementText("New Match Starts In:");
			FrontlineHUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementText));

			AFrontlineGameState* FrontlineGameState = Cast<AFrontlineGameState>(UGameplayStatics::GetGameState(this));
			AFrontlinePlayerState* FrontlinePlayerState = GetPlayerState<AFrontlinePlayerState>();
			if (FrontlineGameState && FrontlinePlayerState)
			{
				TArray<AFrontlinePlayerState*> TopPlayers = FrontlineGameState->TopScoringPlayers;
				FString InfoTextString;
				if (TopPlayers.Num() == 0)
				{
					InfoTextString = FString("There is no winner.");
				}
				else if (TopPlayers.Num() == 1 && TopPlayers[0] == FrontlinePlayerState)
				{
					InfoTextString = FString("You are the winner!");
				}
				else if (TopPlayers.Num() == 1)
				{
					InfoTextString = FString::Printf(TEXT("Winner: \n %s"), *TopPlayers[0]->GetPlayerName());
				}
				else if (TopPlayers.Num() > 1)
				{
					InfoTextString = FString("Players tied for the win:\n");
					for (auto TiedPlayer : TopPlayers)
					{
						InfoTextString.Append(FString::Printf(TEXT("%s\n"), *TiedPlayer->GetPlayerName()));
					}
				}

				FrontlineHUD->Announcement->InfoText->SetText(FText::FromString(InfoTextString));
			}
		}
		FrontlineHUD->SetCharacterOverlayVisibility(false);
	}
	AFrontlineCharacter* FrontlineCharacter = Cast<AFrontlineCharacter>(GetPawn());
	if (FrontlineCharacter && FrontlineCharacter->GetCombat())
	{
		FrontlineCharacter->bDisableGameplay = true;
		FrontlineCharacter->GetCombat()->FireWeapon(false);
	}
}

void AFrontlinePlayerController::ServerCheckMatchState_Implementation()
{
	AFrontlineGameMode* GameMode = Cast<AFrontlineGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		WarmupTime = GameMode->WarmupTime;
		MatchTime = GameMode->MatchTime;
		CooldownTime = GameMode->CooldownTime;
		LevelStartingTime = GameMode->LevelStartingTime;
		MatchState = GameMode->GetMatchState();
		ClientJoinMidGame(MatchState, WarmupTime, MatchTime, CooldownTime, LevelStartingTime);
	}
}

void AFrontlinePlayerController::ClientJoinMidGame_Implementation(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	CooldownTime = Cooldown;
	LevelStartingTime = StartingTime;
	MatchState = StateOfMatch;
	OnMatchStateSet(MatchState);
	if (FrontlineHUD && MatchState == MatchState::WaitingToStart)
	{
		FrontlineHUD->AddAnnouncement();
	}
}
