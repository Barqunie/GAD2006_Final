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
	Info.OutfitIndex = FMath::Max(0, Info.OutfitIndex);
	Info.OutfitLowerIndex = FMath::Max(0, Info.OutfitLowerIndex);
	Info.OutfitShoesIndex = FMath::Max(0, Info.OutfitShoesIndex);
	Info.OutfitUpperIndex = FMath::Max(0, Info.OutfitUpperIndex);
	PlayerInfo = Info;
}

void UDMGameInstance::SetSelectedOutfitIndex(int32 OutfitIndex)
{
	PlayerInfo.OutfitIndex = FMath::Max(0, OutfitIndex);
	PlayerInfo.OutfitLowerIndex = PlayerInfo.OutfitIndex;
	PlayerInfo.OutfitShoesIndex = PlayerInfo.OutfitIndex;
	PlayerInfo.OutfitUpperIndex = PlayerInfo.OutfitIndex;
}

void UDMGameInstance::SetSelectedOutfitPartIndex(EDMCharacterMeshPart MeshPart, int32 PartIndex)
{
	const int32 SafePartIndex = FMath::Max(0, PartIndex);

	switch (MeshPart)
	{
	case EDMCharacterMeshPart::OutfitLower:
		PlayerInfo.OutfitLowerIndex = SafePartIndex;
		break;
	case EDMCharacterMeshPart::OutfitShoes:
		PlayerInfo.OutfitShoesIndex = SafePartIndex;
		break;
	case EDMCharacterMeshPart::OutfitUpper:
		PlayerInfo.OutfitUpperIndex = SafePartIndex;
		break;
	default:
		break;
	}
}

void UDMGameInstance::Host(const FString& MapName, FDMPlayerInfo Info)
{
	Info.OutfitIndex = FMath::Max(0, Info.OutfitIndex);
	Info.OutfitLowerIndex = FMath::Max(0, Info.OutfitLowerIndex);
	Info.OutfitShoesIndex = FMath::Max(0, Info.OutfitShoesIndex);
	Info.OutfitUpperIndex = FMath::Max(0, Info.OutfitUpperIndex);
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
	Info.OutfitIndex = FMath::Max(0, Info.OutfitIndex);
	Info.OutfitLowerIndex = FMath::Max(0, Info.OutfitLowerIndex);
	Info.OutfitShoesIndex = FMath::Max(0, Info.OutfitShoesIndex);
	Info.OutfitUpperIndex = FMath::Max(0, Info.OutfitUpperIndex);
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

