// Fill out your copyright notice in the Description page of Project Settings.


#include "DMGameState.h"

#include "DMPlayerState.h"
#include "Net/UnrealNetwork.h"

ADMGameState::ADMGameState()
{
	bReplicates = true;
}

void ADMGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADMGameState, WinningPlayerIndex);
	DOREPLIFETIME(ADMGameState, LeaderPlayerIndex);
	DOREPLIFETIME(ADMGameState, MatchSecondsRemaining);
	DOREPLIFETIME(ADMGameState, bMatchInProgress);
}

void ADMGameState::OnRep_Winner()
{
	if (WinningPlayerIndex >= 0)
	{
		OnVictory();
	}
}

void ADMGameState::MulticastRestart_Implementation()
{
	OnRestart();
}

ADMPlayerState* ADMGameState::GetPlayerStateByIndex(int32 PlayerIndex) const
{
	for (APlayerState* State : PlayerArray)
	{
		ADMPlayerState* DMState = Cast<ADMPlayerState>(State);

		if (DMState && DMState->PlayerIndex == PlayerIndex)
		{
			return DMState;
		}
	}

	return nullptr;
}

ADMPlayerState* ADMGameState::GetLeaderPlayerState() const
{
	return GetPlayerStateByIndex(LeaderPlayerIndex);
}

