// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "DMGameState.generated.h"

class ADMPlayerState;

/**
 * 
 */
UCLASS()
class AGD_API ADMGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ADMGameState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Winner)
	int32 WinningPlayerIndex = -1;

	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 LeaderPlayerIndex = -1;

	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 MatchSecondsRemaining = 0;

	UPROPERTY(BlueprintReadOnly, Replicated)
	bool bMatchInProgress = false;

	UFUNCTION()
	void OnRep_Winner();

	UFUNCTION(BlueprintImplementableEvent, Category = "Match")
	void OnVictory();

	UFUNCTION(BlueprintImplementableEvent, Category = "Match")
	void OnRestart();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRestart();

	UFUNCTION(BlueprintCallable, Category = "Match")
	ADMPlayerState* GetPlayerStateByIndex(int32 PlayerIndex) const;

	UFUNCTION(BlueprintCallable, Category = "Match")
	ADMPlayerState* GetLeaderPlayerState() const;
};
