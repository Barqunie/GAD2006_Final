// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DMTypes.generated.h"

UENUM(BlueprintType)
enum class EDMCharacterClass : uint8
{
	Sprit,
	Ash
};

USTRUCT(BlueprintType)
struct FDMPlayerInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Nickname = TEXT("Player");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDMCharacterClass CharacterClass = EDMCharacterClass::Sprit;
};

USTRUCT(BlueprintType)
struct FDMScoreboardRow
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 PlayerIndex = -1;

	UPROPERTY(BlueprintReadOnly)
	FString Nickname = TEXT("Player");

	UPROPERTY(BlueprintReadOnly)
	int32 Kills = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 Deaths = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 Score = 0;

	UPROPERTY(BlueprintReadOnly)
	bool bIsLocalPlayer = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsLeader = false;
};
