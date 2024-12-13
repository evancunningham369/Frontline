// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/FrontlinePlayerController.h"
#include "HUD/FrontlineHUD.h"
#include "HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"


void AFrontlinePlayerController::BeginPlay()
{
	Super::BeginPlay();

	FrontlineHUD = Cast<AFrontlineHUD>(GetHUD());
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