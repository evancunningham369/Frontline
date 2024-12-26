// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/FrontlineHUD.h"
#include "GameFramework/PlayerController.h"
#include "HUD/CharacterOverlay.h"
#include "PlayerController/FrontlinePlayerController.h"
#include "HUD/Announcement.h"

void AFrontlineHUD::BeginPlay()
{
	Super::BeginPlay();

	AddCharacterOverlay();
	SetCharacterOverlayVisibility(false);
}

void AFrontlineHUD::AddAnnouncement()
{
	UE_LOG(LogTemp, Warning, TEXT("Adding announcement"))
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && AnnouncementClass && Announcement == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("valid announcement"))
		Announcement = CreateWidget<UAnnouncement>(PlayerController, AnnouncementClass);
		Announcement->AddToViewport();
	}
	else if(!PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("invalid player controller"))
	}
	else if (!Announcement)
	{
		UE_LOG(LogTemp, Warning, TEXT("invalid announcement"))
	}
}

void AFrontlineHUD::AddCharacterOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();

		if (AFrontlinePlayerController* FrontlinePlayerController = Cast<AFrontlinePlayerController>(PlayerController))
		{
			FrontlinePlayerController->SetHUDScore(0.f);
			FrontlinePlayerController->SetHUDDefeats(0);
		}
	}
}

void AFrontlineHUD::SetCharacterOverlayVisibility(bool isVisible)
{
	CharacterOverlay->SetVisibility(isVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void AFrontlineHUD::SetAnnouncementOverlayVisibility(bool isVisible)
{
	Announcement->SetVisibility(isVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void AFrontlineHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewportSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

		float SpreadScaled = CrosshairSpreadMax * HUDPackage.CrosshairSpread;

		if (HUDPackage.CrosshairsCenter)
		{
			DrawCrosshair(HUDPackage.CrosshairsCenter, ViewportCenter, FVector2D::ZeroVector, HUDPackage.CrosshairsColor);
		}
		if (HUDPackage.CrosshairsLeft)
		{
			FVector2D Spread(-SpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.CrosshairsLeft, ViewportCenter, Spread, HUDPackage.CrosshairsColor);
		}
		if (HUDPackage.CrosshairsRight)
		{
			FVector2D Spread(SpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.CrosshairsRight, ViewportCenter, Spread, HUDPackage.CrosshairsColor);
		}
		if (HUDPackage.CrosshairsTop)
		{
			FVector2D Spread(0.f, -SpreadScaled);
			DrawCrosshair(HUDPackage.CrosshairsTop, ViewportCenter, Spread, HUDPackage.CrosshairsColor);
		}
		if (HUDPackage.CrosshairsBottom)
		{
			FVector2D Spread(0.f, SpreadScaled);
			DrawCrosshair(HUDPackage.CrosshairsBottom, ViewportCenter, Spread, HUDPackage.CrosshairsColor);
		}
	}
}

void AFrontlineHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairColor)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2D TextureDrawPoint(
		ViewportCenter.X - (TextureWidth / 2.f) + Spread.X,
		ViewportCenter.Y - (TextureHeight / 2.f) + Spread.Y
	);

	DrawTexture(
		Texture,
		TextureDrawPoint.X,
		TextureDrawPoint.Y,
		TextureWidth,
		TextureHeight,
		0.f,
		0.f,
		1.f,
		1.f,
		CrosshairColor
	);
}
