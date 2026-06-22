// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DMLobbyGameMode.generated.h"

UCLASS()
class AGD_API ADMLobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADMLobbyGameMode();

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void StartMatch(const FString& MapName = TEXT("Arena"));
};
