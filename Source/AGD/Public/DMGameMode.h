// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DMGameMode.generated.h"

class ADMBaseCharacter;
class ADMPlayerState;

/**
 * 
 */
UCLASS()
class AGD_API ADMGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADMGameMode();

	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	UFUNCTION(BlueprintCallable, Category = "Match")
	void HandlePlayerKilled(ADMPlayerState* KillerState, ADMPlayerState* VictimState);

	UFUNCTION(BlueprintCallable, Category = "Match")
	void RestartMatch();

	void RefreshPlayerPawnForSelectedClass(AController* Controller);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deathmatch")
	int32 MatchLengthSeconds = 180;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deathmatch")
	int32 KillsToWin = 15;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deathmatch")
	float RespawnDelay = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DM|Classes")
	TSubclassOf<ADMBaseCharacter> SpritCharacterClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DM|Classes")
	TSubclassOf<ADMBaseCharacter> AshCharacterClass;

private:
	UPROPERTY()
	TArray<TObjectPtr<APlayerController>> ConnectedPlayers;

	FTimerHandle MatchTimerHandle;
	int32 NextPlayerIndex = 0;
	int32 PlayerStartIndex = 0;

	void RegisterPlayer(APlayerController* Player);
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	UClass* GetPawnClassForPlayerState(const ADMPlayerState* State) const;
	void StartMatchTimer();
	void TickMatchTimer();
	void FinishMatch(ADMPlayerState* WinnerState);
	void UpdateLeader();
	void RespawnPlayer(AController* Controller);
	ADMPlayerState* FindLeaderPlayerState() const;
};
