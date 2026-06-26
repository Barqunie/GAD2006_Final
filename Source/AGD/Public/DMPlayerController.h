// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SlateWrapperTypes.h"
#include "GameFramework/PlayerController.h"
#include "DMTypes.h"
#include "DMPlayerController.generated.h"

class ADMBaseCharacter;
class ADMBaseWeapon;
class ADMPlayerState;
class UUserWidget;

UCLASS()
class AGD_API ADMPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void SetupInputComponent() override;

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void SubmitLocalPlayerInfo();

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void SetAndSubmitPlayerInfo(FDMPlayerInfo Info);

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void SelectCharacterClass(EDMCharacterClass CharacterClass);

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void SelectOutfitIndex(int32 OutfitIndex);

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void SelectOutfitPartIndex(EDMCharacterMeshPart MeshPart, int32 PartIndex);

	UFUNCTION(BlueprintPure, Category = "Lobby")
	FDMPlayerInfo GetLocalPlayerInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void UseLobbyCameraByTag(FName CameraTag = TEXT("LobbyCamera"));

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void StartLobbyMatch(const FString& MapName = TEXT("Arena"));

	UFUNCTION(BlueprintPure, Category = "DM|HUD")
	ADMBaseCharacter* GetControlledDMCharacter() const;

	UFUNCTION(BlueprintPure, Category = "DM|HUD")
	ADMBaseWeapon* GetCurrentDMWeapon() const;

	UFUNCTION(BlueprintPure, Category = "DM|HUD")
	ADMPlayerState* GetDMPlayerState() const;

	UFUNCTION(BlueprintPure, Category = "DM|HUD")
	float GetHUDHealthPercent() const;

	UFUNCTION(BlueprintPure, Category = "DM|HUD")
	FText GetHUDHealthText() const;

	UFUNCTION(BlueprintPure, Category = "DM|HUD")
	FText GetHUDAmmoText() const;

	UFUNCTION(BlueprintPure, Category = "DM|HUD")
	FText GetHUDScoreText() const;

	UFUNCTION(BlueprintPure, Category = "DM|HUD")
	FText GetHUDTimerText() const;

	UFUNCTION(BlueprintPure, Category = "DM|HUD")
	FText GetHUDTrapText() const;

	UFUNCTION(BlueprintPure, Category = "DM|HUD")
	TArray<FDMScoreboardRow> GetHUDScoreboardRows() const;

	UFUNCTION(BlueprintPure, Category = "DM|HUD")
	FText GetHUDScoreboardText() const;

	UFUNCTION(BlueprintPure, Category = "DM|HUD")
	bool IsScoreboardVisible() const { return bScoreboardVisible; }

	UFUNCTION(BlueprintPure, Category = "DM|HUD")
	ESlateVisibility GetHUDScoreboardVisibility() const;

	UFUNCTION(BlueprintPure, Category = "DM|HUD")
	ESlateVisibility GetHUDDeathMessageVisibility() const;

	UFUNCTION(BlueprintPure, Category = "DM|HUD")
	ESlateVisibility GetHUDBlindnessVisibility() const;

	UFUNCTION(BlueprintPure, Category = "DM|HUD")
	ESlateVisibility GetHUDEndGameVisibility() const;

	UFUNCTION(BlueprintPure, Category = "DM|HUD")
	bool IsBlinded() const { return bIsBlinded; }

	UFUNCTION(BlueprintPure, Category = "DM|HUD")
	FText GetHUDDeathMessageText() const;

	UFUNCTION(BlueprintPure, Category = "DM|HUD")
	FText GetHUDEndGameTitleText() const;

	UFUNCTION(BlueprintPure, Category = "DM|HUD")
	FText GetHUDEndGameSubtitleText() const;

	UFUNCTION(BlueprintPure, Category = "DM|HUD")
	bool DidLocalPlayerWin() const;

	UFUNCTION(BlueprintImplementableEvent, Category = "DM|HUD")
	void OnScoreboardVisibilityChanged(bool bVisible);

	UFUNCTION(BlueprintImplementableEvent, Category = "DM|Effects")
	void OnBlindnessStarted(float Duration);

	UFUNCTION(BlueprintImplementableEvent, Category = "DM|Effects")
	void OnBlindnessEnded();

	UFUNCTION(BlueprintImplementableEvent, Category = "DM|HUD")
	void OnEndScreenVisibilityChanged(bool bVisible, bool bVictory);

	UFUNCTION(Server, Reliable)
	void ServerSubmitPlayerInfo(FDMPlayerInfo Info);

	UFUNCTION(Server, Reliable)
	void ServerStartLobbyMatch(const FString& MapName);

	UFUNCTION(Client, Reliable)
	void ClientUseMatchInputMode();

	UFUNCTION(Client, Reliable)
	void ClientShowMatchHUD();

	UFUNCTION(Client, Reliable)
	void ClientResubmitPlayerInfoForMatch();

	UFUNCTION(Client, Reliable)
	void ClientApplyBlindness(float Duration);

	UFUNCTION(Client, Reliable)
	void ClientShowEndScreen();

	UFUNCTION(Client, Reliable)
	void ClientHideEndScreen();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DM|HUD")
	TSubclassOf<UUserWidget> MatchHUDClass;

	UPROPERTY(BlueprintReadOnly, Category = "DM|HUD")
	TObjectPtr<UUserWidget> MatchHUDWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DM|HUD")
	TSubclassOf<UUserWidget> EndScreenClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DM|HUD")
	int32 EndScreenZOrder = 100;

	UPROPERTY(BlueprintReadOnly, Category = "DM|HUD")
	TObjectPtr<UUserWidget> EndScreenWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DM|Effects")
	TSubclassOf<UUserWidget> BlindnessWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DM|Effects")
	int32 BlindnessWidgetZOrder = 50;

	UPROPERTY(BlueprintReadOnly, Category = "DM|Effects")
	TObjectPtr<UUserWidget> BlindnessWidget;

private:
	void ShowScoreboard();
	void HideScoreboard();
	void ShowBlindnessWidget();
	void HideBlindnessWidget();
	void EndBlindness();

	bool bScoreboardVisible = false;
	bool bIsBlinded = false;
	bool bEndScreenVisible = false;
	FTimerHandle BlindnessTimerHandle;
};
