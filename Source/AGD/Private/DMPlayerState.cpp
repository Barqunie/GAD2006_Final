// Fill out your copyright notice in the Description page of Project Settings.

#include "DMPlayerState.h"

#include "Net/UnrealNetwork.h"

ADMPlayerState::ADMPlayerState()
{
	bReplicates = true;
}

void ADMPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADMPlayerState, PlayerNickname);
	DOREPLIFETIME(ADMPlayerState, PlayerIndex);
	DOREPLIFETIME(ADMPlayerState, Kills);
	DOREPLIFETIME(ADMPlayerState, Deaths);
	DOREPLIFETIME(ADMPlayerState, MatchScore);
	DOREPLIFETIME(ADMPlayerState, CharacterClass);
}

void ADMPlayerState::ResetMatchStats()
{
	Kills = 0;
	Deaths = 0;
	MatchScore = 0;
}

void ADMPlayerState::ApplyPlayerInfo(const FDMPlayerInfo& Info)
{
	PlayerNickname = Info.Nickname.IsEmpty() ? TEXT("Player") : Info.Nickname;
	CharacterClass = Info.CharacterClass;
	SetPlayerName(PlayerNickname);
}

void ADMPlayerState::AddKill()
{
	Kills++;
	MatchScore++;
}

void ADMPlayerState::AddDeath()
{
	Deaths++;
}
