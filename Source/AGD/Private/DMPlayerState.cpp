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
	DOREPLIFETIME(ADMPlayerState, OutfitIndex);
	DOREPLIFETIME(ADMPlayerState, OutfitLowerIndex);
	DOREPLIFETIME(ADMPlayerState, OutfitShoesIndex);
	DOREPLIFETIME(ADMPlayerState, OutfitUpperIndex);
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
	OutfitIndex = FMath::Max(0, Info.OutfitIndex);
	OutfitLowerIndex = FMath::Max(0, Info.OutfitLowerIndex);
	OutfitShoesIndex = FMath::Max(0, Info.OutfitShoesIndex);
	OutfitUpperIndex = FMath::Max(0, Info.OutfitUpperIndex);
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
