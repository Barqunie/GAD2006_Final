// Fill out your copyright notice in the Description page of Project Settings.


#include "DMGameInstance.h"

#include "Engine/Engine.h"

namespace
{
	FString BuildMainMenuTravelURL(const FString& MapName, bool bListen)
	{
		FString CleanMapName = MapName;
		CleanMapName.TrimStartAndEndInline();

		if (CleanMapName.IsEmpty())
		{
			CleanMapName = TEXT("LobbyMap");
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

void UDMGameInstance::SetPlayerInfo(FDMPlayerInfo Info)
{
	PlayerInfo = Info;
}

void UDMGameInstance::Host(const FString& MapName, FDMPlayerInfo Info)
{
	PlayerInfo = Info;

	UWorld* World = GetWorld();

	if (World == nullptr)
	{
		return;
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("Hosting deathmatch"));
	}

	World->ServerTravel(BuildMainMenuTravelURL(MapName, true));
}

void UDMGameInstance::Join(const FString& Address, FDMPlayerInfo Info)
{
	PlayerInfo = Info;

	UWorld* World = GetWorld();

	if (World == nullptr || World->GetFirstPlayerController() == nullptr)
	{
		return;
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("Joining %s"), *Address));
	}

	World->GetFirstPlayerController()->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
}

