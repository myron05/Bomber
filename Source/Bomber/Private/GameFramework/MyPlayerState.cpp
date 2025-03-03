// Copyright 2021 Yevhenii Selivanov

#include "GameFramework/MyPlayerState.h"
//---
#include "GeneratedMap.h"
#include "GameFramework/MyGameStateBase.h"
#include "Globals/SingletonLibrary.h"
#include "LevelActors/PlayerCharacter.h"
//---
#include "Net/UnrealNetwork.h"

/* ---------------------------------------------------
 *		Protected
 * --------------------------------------------------- */

AMyPlayerState::AMyPlayerState()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

// Set and apply how a player has to look like
void AMyPlayerState::SetCustomPlayerMeshData(const FCustomPlayerMeshData& CustomPlayerMeshData)
{
	PlayerMeshDataInternal = CustomPlayerMeshData;

	if (const auto PlayerCharacter = Cast<APlayerCharacter>(GetPawn()))
	{
		PlayerCharacter->InitMySkeletalMesh(CustomPlayerMeshData);
	}
}

// Set the custom player name by user input
void AMyPlayerState::SetPlayerNameCustom(FName NewName)
{
	if (CustomPlayerNameInternal == NewName
	    || NewName.IsNone())
	{
		return;
	}

	bUseCustomPlayerNames = true;
	CustomPlayerNameInternal = NewName;

	SetPlayerName(NewName.ToString());

	if (OnPlayerNameChanged.IsBound())
	{
		OnPlayerNameChanged.Broadcast(NewName);
	}
}

FName AMyPlayerState::GetPlayerFNameCustom() const
{
	if (CustomPlayerNameInternal.IsNone())
	{
		// Return default name
		const FName PlatformUserName(UKismetSystemLibrary::GetPlatformUserName());
		return PlatformUserName;
	}

	return CustomPlayerNameInternal;
}

// Returns properties that are replicated for the lifetime of the actor channel.
void AMyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EndGameStateInternal);
}

// Called when the game starts. Created widget
void AMyPlayerState::BeginPlay()
{
	Super::BeginPlay();

	// Listen states
	if (AMyGameStateBase* MyGameState = USingletonLibrary::GetMyGameState())
	{
		MyGameState->OnGameStateChanged.AddDynamic(this, &ThisClass::OnGameStateChanged);
		MyGameState->OnAnyPlayerDestroyed.AddDynamic(this, &ThisClass::ServerUpdateEndState);
	}
}

// Listen game states to notify server about ending game for controlled player
void AMyPlayerState::OnGameStateChanged(ECurrentGameState CurrentGameState)
{
	switch (CurrentGameState)
	{
		case ECurrentGameState::GameStarting:
		{
			EndGameStateInternal = EEndGameState::None;
			break;
		}
		case ECurrentGameState::EndGame:
		{
			ServerUpdateEndState();
			break;
		}
		default:
			break;
	}
}

// Updated result of the game for controlled player after ending the game. Called when one of players is destroying
void AMyPlayerState::ServerUpdateEndState_Implementation()
{
	const ECurrentGameState CurrentGameState = AMyGameStateBase::GetCurrentGameState(this);
	if (CurrentGameState == ECurrentGameState::None     // is not valid game state, nullptr or not fully initialized
	    || EndGameStateInternal != EEndGameState::None) // end state was set already for current game
	{
		return;
	}

	// handle timer is 0
	if (CurrentGameState == ECurrentGameState::EndGame) // game was finished
	{
		EndGameStateInternal = EEndGameState::Draw;
		return;
	}

	if (CurrentGameState != ECurrentGameState::InGame)
	{
		return;
	}

	// Game is running

	// locals
	bool bUpdateGameState = false;
	bool bUpdateEndGameState = false;
	const int32 PlayerNum = USingletonLibrary::GetAlivePlayersNum();
	const APawn* PawnOwner = GetPawn();
	if (!IS_VALID(PawnOwner)) // is dead owner
	{
		if (PlayerNum <= 0) // last players were blasted together
		{
			EndGameStateInternal = EEndGameState::Draw;
			bUpdateGameState = true; // no players to play, game ended
			bUpdateEndGameState = true;
		}
		else
		{
			EndGameStateInternal = EEndGameState::Lose;
			bUpdateGameState = PlayerNum == 1;
			bUpdateEndGameState = true;
		}
	}
	else if (PlayerNum == 1) // is alive owner and is the last player
	{
		if (PawnOwner->GetController<APlayerController>()) // is player
		{
			EndGameStateInternal = EEndGameState::Win;
		}
		bUpdateGameState = true; // we have winner, game ended
		bUpdateEndGameState = true;
	}

	// Need to notify that the game was ended
	if (bUpdateGameState)
	{
		USingletonLibrary::GetMyGameState()->ServerSetGameState(ECurrentGameState::EndGame);
	}

	if (bUpdateEndGameState
	    && OnEndGameStateChanged.IsBound())
	{
		OnEndGameStateChanged.Broadcast(EndGameStateInternal);
	}
}
