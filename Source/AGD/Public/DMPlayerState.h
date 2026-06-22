// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "DMTypes.h"
#include "DMPlayerState.generated.h"

UCLASS()
class AGD_API ADMPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ADMPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadOnly, Replicated)
	FString PlayerNickname = TEXT("Player");

	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 PlayerIndex = -1;

	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 Kills = 0;

	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 Deaths = 0;

	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 MatchScore = 0;

	UPROPERTY(BlueprintReadOnly, Replicated)
	EDMCharacterClass CharacterClass = EDMCharacterClass::Sprit;

	UFUNCTION(BlueprintCallable)
	void ResetMatchStats();

	UFUNCTION(BlueprintCallable)
	void ApplyPlayerInfo(const FDMPlayerInfo& Info);

	void AddKill();
	void AddDeath();
};
