// Fill out your copyright notice in the Description page of Project Settings.

#include "DMLobbyGameMode.h"

#include "DMPlayerController.h"
#include "DMPlayerState.h"

namespace
{
	FString BuildLobbyTravelURL(const FString& MapName, bool bListen)
	{
		FString CleanMapName = MapName;
		CleanMapName.TrimStartAndEndInline();

		if (CleanMapName.IsEmpty())
		{
			CleanMapName = TEXT("Arena");
		}

		FString MapPath;

		if (CleanMapName.StartsWith(TEXT("/Game/")))
		{
			MapPath = CleanMapName;
		}
		else if (CleanMapName.Contains(TEXT("/")))
		{
			MapPath = FString::Printf(TEXT("/Game/%s"), *CleanMapName);
		}
		else
		{
			MapPath = FString::Printf(TEXT("/Game/%s"), *CleanMapName);
		}

		return bListen ? FString::Printf(TEXT("%s?listen"), *MapPath) : MapPath;
	}
}

ADMLobbyGameMode::ADMLobbyGameMode()
{
	PlayerControllerClass = ADMPlayerController::StaticClass();
	PlayerStateClass = ADMPlayerState::StaticClass();
	DefaultPawnClass = nullptr;
}

void ADMLobbyGameMode::StartMatch(const FString& MapName)
{
	UWorld* World = GetWorld();

	if (World == nullptr)
	{
		return;
	}

	const FString TargetMap = MapName.IsEmpty() ? TEXT("Arena") : MapName;
	World->ServerTravel(BuildLobbyTravelURL(TargetMap, true));
}
