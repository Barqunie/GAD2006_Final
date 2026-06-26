// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DMTypes.generated.h"

class USkeletalMesh;

UENUM(BlueprintType)
enum class EDMCharacterClass : uint8
{
	Sprit,
	Ash
};

UENUM(BlueprintType)
enum class EDMCharacterMeshPart : uint8
{
	Torso,
	Feet,
	Hands,
	Head,
	Eyes,
	Teeth,
	Hair,
	Legs,
	OutfitLower,
	OutfitShoes,
	OutfitUpper
};

USTRUCT(BlueprintType)
struct FDMModularCharacterMeshes
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Character|Modular")
	TObjectPtr<USkeletalMesh> Torso = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Character|Modular")
	TObjectPtr<USkeletalMesh> Feet = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Character|Modular")
	TObjectPtr<USkeletalMesh> Hands = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Character|Modular")
	TObjectPtr<USkeletalMesh> Head = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Character|Modular")
	TObjectPtr<USkeletalMesh> Eyes = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Character|Modular")
	TObjectPtr<USkeletalMesh> Teeth = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Character|Modular")
	TObjectPtr<USkeletalMesh> Hair = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Character|Modular")
	TObjectPtr<USkeletalMesh> Legs = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Character|Modular")
	TObjectPtr<USkeletalMesh> OutfitLower = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Character|Modular")
	TObjectPtr<USkeletalMesh> OutfitShoes = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Character|Modular")
	TObjectPtr<USkeletalMesh> OutfitUpper = nullptr;
};

USTRUCT(BlueprintType)
struct FDMOutfitMeshes
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Character|Outfit")
	TObjectPtr<USkeletalMesh> Lower = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Character|Outfit")
	TObjectPtr<USkeletalMesh> Shoes = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Character|Outfit")
	TObjectPtr<USkeletalMesh> Upper = nullptr;
};

USTRUCT(BlueprintType)
struct FDMPlayerInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Nickname = TEXT("Player");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDMCharacterClass CharacterClass = EDMCharacterClass::Sprit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 OutfitIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 OutfitLowerIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 OutfitShoesIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 OutfitUpperIndex = 0;
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
