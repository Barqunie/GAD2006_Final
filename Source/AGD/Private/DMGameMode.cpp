// Fill out your copyright notice in the Description page of Project Settings.


#include "DMGameMode.h"

#include "DMBaseCharacter.h"
#include "DMGameState.h"
#include "DMPlayerController.h"
#include "DMPlayerState.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "TimerManager.h"

ADMGameMode::ADMGameMode()
{
	DefaultPawnClass = ADMBaseCharacter::StaticClass();
	PlayerControllerClass = ADMPlayerController::StaticClass();
	PlayerStateClass = ADMPlayerState::StaticClass();
	GameStateClass = ADMGameState::StaticClass();
}

void ADMGameMode::BeginPlay()
{
	Super::BeginPlay();
	StartMatchTimer();
}

void ADMGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	RegisterPlayer(NewPlayer);
}

void ADMGameMode::RegisterPlayer(APlayerController* Player)
{
	if (Player == nullptr || ConnectedPlayers.Contains(Player))
	{
		return;
	}

	ADMPlayerState* State = Player->GetPlayerState<ADMPlayerState>();

	if (State == nullptr)
	{
		return;
	}

	State->PlayerIndex = NextPlayerIndex++;
	State->ResetMatchStats();
	ConnectedPlayers.Add(Player);
}

UClass* ADMGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	ADMPlayerState* State = InController ? InController->GetPlayerState<ADMPlayerState>() : nullptr;
	UClass* SelectedPawnClass = GetPawnClassForPlayerState(State);

	return SelectedPawnClass ? SelectedPawnClass : Super::GetDefaultPawnClassForController_Implementation(InController);
}

UClass* ADMGameMode::GetPawnClassForPlayerState(const ADMPlayerState* State) const
{
	if (State)
	{
		if (State->CharacterClass == EDMCharacterClass::Ash && AshCharacterClass)
		{
			return AshCharacterClass;
		}

		if (State->CharacterClass == EDMCharacterClass::Sprit && SpritCharacterClass)
		{
			return SpritCharacterClass;
		}
	}

	return DefaultPawnClass;
}

void ADMGameMode::RefreshPlayerPawnForSelectedClass(AController* Controller)
{
	if (Controller == nullptr)
	{
		return;
	}

	const ADMPlayerState* State = Controller->GetPlayerState<ADMPlayerState>();
	UClass* DesiredPawnClass = GetPawnClassForPlayerState(State);
	APawn* CurrentPawn = Controller->GetPawn();

	if (CurrentPawn && DesiredPawnClass && CurrentPawn->IsA(DesiredPawnClass))
	{
		return;
	}

	if (CurrentPawn)
	{
		Controller->UnPossess();
		CurrentPawn->Destroy();
	}

	Controller->StartSpot.Reset();
	RestartPlayer(Controller);
}

AActor* ADMGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	const FName PreferredTag = *FString::Printf(TEXT("DM%d"), PlayerStartIndex++ % 8);

	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		APlayerStart* Start = Cast<APlayerStart>(*It);

		if (Start && Start->PlayerStartTag == PreferredTag)
		{
			return Start;
		}
	}

	return Super::ChoosePlayerStart_Implementation(Player);
}

void ADMGameMode::StartMatchTimer()
{
	ADMGameState* DMGameState = GetGameState<ADMGameState>();

	if (DMGameState)
	{
		DMGameState->WinningPlayerIndex = -1;
		DMGameState->LeaderPlayerIndex = -1;
		DMGameState->MatchSecondsRemaining = MatchLengthSeconds;
		DMGameState->bMatchInProgress = true;
	}

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(MatchTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(MatchTimerHandle, this, &ADMGameMode::TickMatchTimer, 1.0f, true);
	}
}

void ADMGameMode::TickMatchTimer()
{
	ADMGameState* DMGameState = GetGameState<ADMGameState>();

	if (DMGameState == nullptr || !DMGameState->bMatchInProgress || DMGameState->WinningPlayerIndex >= 0)
	{
		return;
	}

	DMGameState->MatchSecondsRemaining = FMath::Max(0, DMGameState->MatchSecondsRemaining - 1);
	UpdateLeader();

	if (DMGameState->MatchSecondsRemaining <= 0)
	{
		FinishMatch(FindLeaderPlayerState());
	}
}

void ADMGameMode::HandlePlayerKilled(ADMPlayerState* KillerState, ADMPlayerState* VictimState)
{
	ADMGameState* DMGameState = GetGameState<ADMGameState>();

	if (DMGameState == nullptr || !DMGameState->bMatchInProgress || DMGameState->WinningPlayerIndex >= 0 || VictimState == nullptr)
	{
		return;
	}

	VictimState->AddDeath();

	if (KillerState && KillerState != VictimState)
	{
		KillerState->AddKill();
	}

	UpdateLeader();

	if (KillerState && KillerState->Kills >= KillsToWin)
	{
		FinishMatch(KillerState);
		return;
	}

	AController* VictimController = Cast<AController>(VictimState->GetOwner());

	if (VictimController && GetWorld())
	{
		FTimerHandle RespawnTimer;
		FTimerDelegate RespawnDelegate;
		RespawnDelegate.BindUObject(this, &ADMGameMode::RespawnPlayer, VictimController);
		GetWorld()->GetTimerManager().SetTimer(RespawnTimer, RespawnDelegate, RespawnDelay, false);
	}
}

void ADMGameMode::RespawnPlayer(AController* Controller)
{
	ADMGameState* DMGameState = GetGameState<ADMGameState>();

	if (Controller == nullptr || DMGameState == nullptr || !DMGameState->bMatchInProgress || DMGameState->WinningPlayerIndex >= 0)
	{
		return;
	}

	APawn* OldPawn = Controller->GetPawn();

	if (OldPawn)
	{
		Controller->UnPossess();
		OldPawn->Destroy();
	}

	Controller->StartSpot.Reset();
	RestartPlayer(Controller);
}

void ADMGameMode::UpdateLeader()
{
	ADMGameState* DMGameState = GetGameState<ADMGameState>();
	ADMPlayerState* Leader = FindLeaderPlayerState();

	if (DMGameState)
	{
		DMGameState->LeaderPlayerIndex = Leader ? Leader->PlayerIndex : -1;
	}
}

ADMPlayerState* ADMGameMode::FindLeaderPlayerState() const
{
	ADMPlayerState* BestState = nullptr;

	for (APlayerController* Player : ConnectedPlayers)
	{
		if (Player == nullptr)
		{
			continue;
		}

		ADMPlayerState* State = Player->GetPlayerState<ADMPlayerState>();

		if (State == nullptr)
		{
			continue;
		}

		if (BestState == nullptr || State->MatchScore > BestState->MatchScore)
		{
			BestState = State;
		}
	}

	return BestState;
}

void ADMGameMode::FinishMatch(ADMPlayerState* WinnerState)
{
	ADMGameState* DMGameState = GetGameState<ADMGameState>();

	if (DMGameState == nullptr || DMGameState->WinningPlayerIndex >= 0)
	{
		return;
	}

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(MatchTimerHandle);
	}

	DMGameState->bMatchInProgress = false;
	DMGameState->WinningPlayerIndex = WinnerState ? WinnerState->PlayerIndex : -1;
	DMGameState->LeaderPlayerIndex = DMGameState->WinningPlayerIndex;
	DMGameState->OnVictory();

	for (APlayerController* Player : ConnectedPlayers)
	{
		if (ADMPlayerController* DMController = Cast<ADMPlayerController>(Player))
		{
			DMController->ClientShowEndScreen();
		}
	}
}

void ADMGameMode::RestartMatch()
{
	PlayerStartIndex = 0;

	for (APlayerController* Player : ConnectedPlayers)
	{
		if (Player == nullptr)
		{
			continue;
		}

		if (ADMPlayerController* DMController = Cast<ADMPlayerController>(Player))
		{
			DMController->ClientHideEndScreen();
		}

		ADMPlayerState* State = Player->GetPlayerState<ADMPlayerState>();

		if (State)
		{
			State->ResetMatchStats();
		}

		APawn* Pawn = Player->GetPawn();

		if (Pawn)
		{
			Player->UnPossess();
			Pawn->Destroy();
		}

		Player->StartSpot.Reset();
		RestartPlayer(Player);
	}

	ADMGameState* DMGameState = GetGameState<ADMGameState>();

	if (DMGameState)
	{
		DMGameState->MulticastRestart();
	}

	StartMatchTimer();
}

