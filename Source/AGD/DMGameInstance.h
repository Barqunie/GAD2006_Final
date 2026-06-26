// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DMTypes.h"
#include "DMGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class AGD_API UDMGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void SetPlayerInfo(FDMPlayerInfo Info);

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void SetSelectedOutfitIndex(int32 OutfitIndex);

	UFUNCTION(BlueprintPure, Category = "Lobby")
	int32 GetSelectedOutfitIndex() const { return PlayerInfo.OutfitIndex; }

	UFUNCTION(BlueprintCallable, Category = "Network")
	void Host(const FString& MapName, FDMPlayerInfo Info);

	UFUNCTION(BlueprintCallable, Category = "Network")
	void Join(const FString& Address, FDMPlayerInfo Info);

	UPROPERTY(BlueprintReadWrite, Category = "Player")
	FDMPlayerInfo PlayerInfo;
};
