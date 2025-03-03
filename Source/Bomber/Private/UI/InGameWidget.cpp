﻿// Copyright 2021 Yevhenii Selivanov.

#include "UI/InGameWidget.h"
//---
#include "SoundsManager.h"
#include "Controllers/MyPlayerController.h"
#include "GameFramework/MyGameStateBase.h"
#include "Globals/SingletonLibrary.h"
#include "UI/MyHUD.h"
#include "UI/SettingsWidget.h"
//---
#include "Components/Button.h"

// Shows the in game menu.
void UInGameWidget::ShowInGameMenu_Implementation()
{
	if (IsVisibleInGameMenu())
	{
		// Is already shown
		return;
	}

	// Show mouse cursor
	if (AMyPlayerController* MyPlayerController = USingletonLibrary::GetMyPlayerController())
	{
		MyPlayerController->SetMouseVisibility(true);
	}

	if (OnToggledInGameMenu.IsBound())
	{
		OnToggledInGameMenu.Broadcast(true);
	}

	// Blueprint implementation
	// ...
}

// Hide the in game menu.
void UInGameWidget::HideInGameMenu_Implementation()
{
	if (!IsVisibleInGameMenu())
	{
		// Is already hidden
		return;
	}

	// Hide mouse cursor
	if (AMyPlayerController* MyPlayerController = USingletonLibrary::GetMyPlayerController())
	{
		MyPlayerController->SetMouseVisibility(false);
	}

	if (OnToggledInGameMenu.IsBound())
	{
		OnToggledInGameMenu.Broadcast(false);
	}

	// Blueprint implementation
	// ...
}

// Flip-floppy show and hide the end game state window
void UInGameWidget::ToggleInGameMenu()
{
	const ECurrentGameState CurrentGameState = AMyGameStateBase::GetCurrentGameState(GetWorld());
	if (CurrentGameState != ECurrentGameState::InGame
	    && CurrentGameState != ECurrentGameState::EndGame)
	{
		return;
	}

	// Play the sound
	if (USoundsManager* SoundsManager = USingletonLibrary::GetSoundsManager())
	{
		SoundsManager->PlayUIClickSFX();
	}

	if (IsVisibleInGameMenu())
	{
		HideInGameMenu();
	}
	else
	{
		ShowInGameMenu();
	}
}

// Called after the underlying slate widget is constructed. May be called multiple times due to adding and removing from the hierarchy.
void UInGameWidget::NativeConstruct()
{
	// Call the Blueprint "Event Construct" node
	Super::NativeConstruct();

	// Hide that widget by default
	SetVisibility(ESlateVisibility::Collapsed);

	// Listen changing the visibility of this widget
	OnVisibilityChanged.AddUniqueDynamic(this, &ThisClass::OnWidgetVisibilityChanged);

	// Listen states to spawn widgets
	if (AMyGameStateBase* MyGameState = USingletonLibrary::GetMyGameState())
	{
		MyGameState->OnGameStateChanged.AddUniqueDynamic(this, &ThisClass::OnGameStateChanged);
	}

	// Listen to toggle the game state widget when is requested
	if (AMyHUD* MyHUD = USingletonLibrary::GetMyHUD())
	{
		MyHUD->OnClose.AddUniqueDynamic(this, &ThisClass::ToggleInGameMenu);
	}

	if (RestartButtonInternal)
	{
		RestartButtonInternal->OnPressed.AddUniqueDynamic(this, &ThisClass::OnRestartButtonPressed);
	}

	if (MenuButtonInternal)
	{
		MenuButtonInternal->OnPressed.AddUniqueDynamic(this, &ThisClass::OnMenuButtonPressed);
	}

	if (SettingsButtonInternal)
	{
		SettingsButtonInternal->OnPressed.AddUniqueDynamic(this, &ThisClass::OnSettingsButtonPressed);
	}
}

// Updates appearance dynamically in the editor
void UInGameWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
}

// Launch 'Three-two-one-GO' timer.
void UInGameWidget::LaunchStartingCountdown_Implementation()
{
	// Blueprint implementation
	// ...
}

// Launch the main timer that count the seconds to the game ending.
void UInGameWidget::LaunchInGameCountdown_Implementation()
{
	// Blueprint implementation
	// ...
}

// Called when the visibility of this widgets was changed
void UInGameWidget::OnWidgetVisibilityChanged(ESlateVisibility InVisibility)
{
	if (InVisibility != ESlateVisibility::Visible)
	{
		HideInGameMenu();
	}
}

// Called when the current game state was changed
void UInGameWidget::OnGameStateChanged_Implementation(ECurrentGameState CurrentGameState)
{
	switch (CurrentGameState)
	{
		case ECurrentGameState::Menu:
		{
			SetVisibility(ESlateVisibility::Collapsed);
			break;
		}
		case ECurrentGameState::GameStarting:
		{
			SetVisibility(ESlateVisibility::Visible);
			HideInGameMenu();
			LaunchStartingCountdown();
			break;
		}
		case ECurrentGameState::EndGame:
		{
			ShowInGameMenu();
			break;
		}
		case ECurrentGameState::InGame:
		{
			LaunchInGameCountdown();
			HideInGameMenu();
			break;
		}
		default:
			break;
	}
}

// Is called when player pressed the button to restart the game
void UInGameWidget::OnRestartButtonPressed()
{
	// Play the sound
	if (USoundsManager* SoundsManager = USingletonLibrary::GetSoundsManager())
	{
		SoundsManager->PlayUIClickSFX();
	}

	if (AMyPlayerController* MyPC = USingletonLibrary::GetMyPlayerController())
	{
		MyPC->SetGameStartingState();
	}
}

// Is called when player pressed the button to go back to the Main Menu
void UInGameWidget::OnMenuButtonPressed()
{
	// Play the sound
	if (USoundsManager* SoundsManager = USingletonLibrary::GetSoundsManager())
	{
		SoundsManager->PlayUIClickSFX();
	}

	if (AMyPlayerController* MyPC = USingletonLibrary::GetMyPlayerController())
	{
		MyPC->SetMenuState();
	}
}

// Is called when player pressed the button to open in-game Settings
void UInGameWidget::OnSettingsButtonPressed()
{
	if (USettingsWidget* SettingsWidget = USingletonLibrary::GetSettingsWidget())
	{
		SettingsWidget->OpenSettings();
	}
}
