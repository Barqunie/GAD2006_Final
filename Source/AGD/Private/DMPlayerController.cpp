// Fill out your copyright notice in the Description page of Project Settings.

#include "DMPlayerController.h"

#include "../DMGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraActor.h"
#include "DMBaseCharacter.h"
#include "DMBaseWeapon.h"
#include "DMGameState.h"
#include "DMGameMode.h"
#include "DMLobbyGameMode.h"
#include "DMPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

void ADMPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		SubmitLocalPlayerInfo();
	}
}

void ADMPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (InPawn && GetWorld() && GetWorld()->GetAuthGameMode<ADMGameMode>())
	{
		ClientUseMatchInputMode();
		ClientShowMatchHUD();
		ClientResubmitPlayerInfoForMatch();
	}
}

void ADMPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		InputComponent->BindAction(TEXT("Scoreboard"), IE_Pressed, this, &ADMPlayerController::ShowScoreboard);
		InputComponent->BindAction(TEXT("Scoreboard"), IE_Released, this, &ADMPlayerController::HideScoreboard);
	}
}

void ADMPlayerController::SubmitLocalPlayerInfo()
{
	UDMGameInstance* DMGameInstance = GetGameInstance<UDMGameInstance>();

	if (DMGameInstance == nullptr)
	{
		return;
	}

	ServerSubmitPlayerInfo(DMGameInstance->PlayerInfo);
}

void ADMPlayerController::SetAndSubmitPlayerInfo(FDMPlayerInfo Info)
{
	Info.OutfitIndex = FMath::Max(0, Info.OutfitIndex);
	Info.OutfitLowerIndex = FMath::Max(0, Info.OutfitLowerIndex);
	Info.OutfitShoesIndex = FMath::Max(0, Info.OutfitShoesIndex);
	Info.OutfitUpperIndex = FMath::Max(0, Info.OutfitUpperIndex);

	UDMGameInstance* DMGameInstance = GetGameInstance<UDMGameInstance>();

	if (DMGameInstance)
	{
		DMGameInstance->SetPlayerInfo(Info);
	}

	ServerSubmitPlayerInfo(Info);
}

void ADMPlayerController::SelectCharacterClass(EDMCharacterClass CharacterClass)
{
	FDMPlayerInfo Info = GetLocalPlayerInfo();
	Info.CharacterClass = CharacterClass;

	SetAndSubmitPlayerInfo(Info);
}

void ADMPlayerController::SelectOutfitIndex(int32 OutfitIndex)
{
	FDMPlayerInfo Info = GetLocalPlayerInfo();
	Info.OutfitIndex = FMath::Max(0, OutfitIndex);
	Info.OutfitLowerIndex = Info.OutfitIndex;
	Info.OutfitShoesIndex = Info.OutfitIndex;
	Info.OutfitUpperIndex = Info.OutfitIndex;

	SetAndSubmitPlayerInfo(Info);
}

void ADMPlayerController::SelectOutfitPartIndex(EDMCharacterMeshPart MeshPart, int32 PartIndex)
{
	FDMPlayerInfo Info = GetLocalPlayerInfo();
	const int32 SafePartIndex = FMath::Max(0, PartIndex);

	switch (MeshPart)
	{
	case EDMCharacterMeshPart::OutfitLower:
		Info.OutfitLowerIndex = SafePartIndex;
		break;
	case EDMCharacterMeshPart::OutfitShoes:
		Info.OutfitShoesIndex = SafePartIndex;
		break;
	case EDMCharacterMeshPart::OutfitUpper:
		Info.OutfitUpperIndex = SafePartIndex;
		break;
	default:
		return;
	}

	SetAndSubmitPlayerInfo(Info);
}

FDMPlayerInfo ADMPlayerController::GetLocalPlayerInfo() const
{
	const UDMGameInstance* DMGameInstance = GetGameInstance<UDMGameInstance>();

	if (DMGameInstance)
	{
		return DMGameInstance->PlayerInfo;
	}

	return FDMPlayerInfo();
}

void ADMPlayerController::UseLobbyCameraByTag(FName CameraTag)
{
	if (!IsLocalController())
	{
		return;
	}

	AActor* LobbyCamera = nullptr;
	TArray<AActor*> TaggedActors;
	UGameplayStatics::GetAllActorsWithTag(this, CameraTag, TaggedActors);

	for (AActor* Actor : TaggedActors)
	{
		if (Actor && Actor->IsA<ACameraActor>())
		{
			LobbyCamera = Actor;
			break;
		}
	}

	if (LobbyCamera == nullptr)
	{
		TArray<AActor*> CameraActors;
		UGameplayStatics::GetAllActorsOfClass(this, ACameraActor::StaticClass(), CameraActors);
		LobbyCamera = CameraActors.Num() > 0 ? CameraActors[0] : nullptr;
	}

	if (LobbyCamera)
	{
		SetViewTarget(LobbyCamera);
	}

	bShowMouseCursor = true;
	SetIgnoreMoveInput(false);
	SetIgnoreLookInput(false);

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
}

void ADMPlayerController::ServerSubmitPlayerInfo_Implementation(FDMPlayerInfo Info)
{
	ADMPlayerState* DMPlayerState = GetPlayerState<ADMPlayerState>();

	if (DMPlayerState)
	{
		DMPlayerState->ApplyPlayerInfo(Info);
	}

	ADMGameMode* DMGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ADMGameMode>() : nullptr;

	if (DMGameMode)
	{
		DMGameMode->RefreshPlayerPawnForSelectedClass(this);
	}

	if (ADMBaseCharacter* DMCharacter = Cast<ADMBaseCharacter>(GetPawn()))
	{
		DMCharacter->ApplyPlayerCustomizationFromPlayerState();
	}
}

ADMBaseCharacter* ADMPlayerController::GetControlledDMCharacter() const
{
	return Cast<ADMBaseCharacter>(GetPawn());
}

ADMBaseWeapon* ADMPlayerController::GetCurrentDMWeapon() const
{
	const ADMBaseCharacter* DMCharacter = GetControlledDMCharacter();
	return DMCharacter ? DMCharacter->CurrentWeapon : nullptr;
}

ADMPlayerState* ADMPlayerController::GetDMPlayerState() const
{
	return GetPlayerState<ADMPlayerState>();
}

float ADMPlayerController::GetHUDHealthPercent() const
{
	const ADMBaseCharacter* DMCharacter = GetControlledDMCharacter();

	if (DMCharacter == nullptr || DMCharacter->MaxHealth <= 0.0f)
	{
		return 0.0f;
	}

	return FMath::Clamp(DMCharacter->Health / DMCharacter->MaxHealth, 0.0f, 1.0f);
}

FText ADMPlayerController::GetHUDHealthText() const
{
	const ADMBaseCharacter* DMCharacter = GetControlledDMCharacter();

	if (DMCharacter == nullptr)
	{
		return FText::FromString(TEXT("0 / 0"));
	}

	return FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), DMCharacter->Health, DMCharacter->MaxHealth));
}

FText ADMPlayerController::GetHUDAmmoText() const
{
	const ADMBaseWeapon* Weapon = GetCurrentDMWeapon();

	if (Weapon == nullptr)
	{
		return FText::FromString(TEXT("-- / --"));
	}

	return FText::FromString(FString::Printf(TEXT("%d / %d"), Weapon->CurrentAmmo, Weapon->ReserveAmmo));
}

FText ADMPlayerController::GetHUDScoreText() const
{
	const ADMPlayerState* State = GetDMPlayerState();

	if (State == nullptr)
	{
		return FText::FromString(TEXT("K 0  D 0"));
	}

	return FText::FromString(FString::Printf(TEXT("K %d  D %d"), State->Kills, State->Deaths));
}

FText ADMPlayerController::GetHUDTimerText() const
{
	const ADMGameState* DMGameState = GetWorld() ? GetWorld()->GetGameState<ADMGameState>() : nullptr;

	if (DMGameState == nullptr)
	{
		return FText::FromString(TEXT("00:00"));
	}

	const int32 Seconds = FMath::Max(0, DMGameState->MatchSecondsRemaining);
	const int32 MinutesPart = Seconds / 60;
	const int32 SecondsPart = Seconds % 60;

	return FText::FromString(FString::Printf(TEXT("%02d:%02d"), MinutesPart, SecondsPart));
}

FText ADMPlayerController::GetHUDTrapText() const
{
	const ADMBaseCharacter* DMCharacter = GetControlledDMCharacter();
	if (DMCharacter == nullptr || !DMCharacter->HasAvailableTrap())
	{
		return FText::FromString(TEXT("Trap: None"));
	}

	return FText::Format(
		FText::FromString(TEXT("Trap: {0}")),
		DMCharacter->GetAvailableTrapDisplayName());
}

TArray<FDMScoreboardRow> ADMPlayerController::GetHUDScoreboardRows() const
{
	TArray<FDMScoreboardRow> Rows;
	const ADMGameState* DMGameState = GetWorld() ? GetWorld()->GetGameState<ADMGameState>() : nullptr;
	const ADMPlayerState* LocalState = GetDMPlayerState();

	if (DMGameState == nullptr)
	{
		return Rows;
	}

	for (APlayerState* StateEntry : DMGameState->PlayerArray)
	{
		const ADMPlayerState* DMState = Cast<ADMPlayerState>(StateEntry);

		if (DMState == nullptr)
		{
			continue;
		}

		FDMScoreboardRow Row;
		Row.PlayerIndex = DMState->PlayerIndex;
		Row.Nickname = DMState->PlayerNickname;
		Row.Kills = DMState->Kills;
		Row.Deaths = DMState->Deaths;
		Row.Score = DMState->MatchScore;
		Row.bIsLocalPlayer = DMState == LocalState;
		Row.bIsLeader = DMState->PlayerIndex >= 0 && DMState->PlayerIndex == DMGameState->LeaderPlayerIndex;
		Rows.Add(Row);
	}

	Rows.Sort([](const FDMScoreboardRow& Left, const FDMScoreboardRow& Right)
	{
		if (Left.Score != Right.Score)
		{
			return Left.Score > Right.Score;
		}

		if (Left.Kills != Right.Kills)
		{
			return Left.Kills > Right.Kills;
		}

		if (Left.Deaths != Right.Deaths)
		{
			return Left.Deaths < Right.Deaths;
		}

		return Left.PlayerIndex < Right.PlayerIndex;
	});

	return Rows;
}

FText ADMPlayerController::GetHUDScoreboardText() const
{
	const TArray<FDMScoreboardRow> Rows = GetHUDScoreboardRows();
	FString ScoreboardString = TEXT("PLAYER              K   D   SCORE\n");

	for (const FDMScoreboardRow& Row : Rows)
	{
		const FString LocalMarker = Row.bIsLocalPlayer ? TEXT("> ") : TEXT("  ");
		const FString LeaderMarker = Row.bIsLeader ? TEXT("*") : TEXT(" ");
		ScoreboardString += FString::Printf(
			TEXT("%s%s%-16s  %2d  %2d    %2d\n"),
			*LocalMarker,
			*LeaderMarker,
			*Row.Nickname.Left(16),
			Row.Kills,
			Row.Deaths,
			Row.Score);
	}

	return FText::FromString(ScoreboardString);
}

ESlateVisibility ADMPlayerController::GetHUDScoreboardVisibility() const
{
	return bScoreboardVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
}

ESlateVisibility ADMPlayerController::GetHUDDeathMessageVisibility() const
{
	const ADMBaseCharacter* DMCharacter = GetControlledDMCharacter();
	return DMCharacter && DMCharacter->bIsDead ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
}

ESlateVisibility ADMPlayerController::GetHUDBlindnessVisibility() const
{
	return bIsBlinded ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
}

ESlateVisibility ADMPlayerController::GetHUDEndGameVisibility() const
{
	return bEndScreenVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
}

FText ADMPlayerController::GetHUDDeathMessageText() const
{
	return FText::FromString(TEXT("YOU DIED"));
}

bool ADMPlayerController::DidLocalPlayerWin() const
{
	const ADMGameState* DMGameState = GetWorld() ? GetWorld()->GetGameState<ADMGameState>() : nullptr;
	const ADMPlayerState* LocalState = GetDMPlayerState();

	return DMGameState
		&& LocalState
		&& DMGameState->WinningPlayerIndex >= 0
		&& LocalState->PlayerIndex == DMGameState->WinningPlayerIndex;
}

FText ADMPlayerController::GetHUDEndGameTitleText() const
{
	const ADMGameState* DMGameState = GetWorld() ? GetWorld()->GetGameState<ADMGameState>() : nullptr;

	if (DMGameState == nullptr || DMGameState->WinningPlayerIndex < 0)
	{
		return FText::FromString(TEXT("DRAW"));
	}

	return DidLocalPlayerWin() ? FText::FromString(TEXT("VICTORY")) : FText::FromString(TEXT("DEFEAT"));
}

FText ADMPlayerController::GetHUDEndGameSubtitleText() const
{
	const ADMGameState* DMGameState = GetWorld() ? GetWorld()->GetGameState<ADMGameState>() : nullptr;

	if (DMGameState == nullptr || DMGameState->WinningPlayerIndex < 0)
	{
		return FText::FromString(TEXT("No winner"));
	}

	const ADMPlayerState* WinnerState = DMGameState->GetPlayerStateByIndex(DMGameState->WinningPlayerIndex);
	if (WinnerState == nullptr)
	{
		return FText::FromString(TEXT("Winner unknown"));
	}

	return FText::FromString(FString::Printf(
		TEXT("Winner: %s   K %d  D %d  Score %d"),
		*WinnerState->PlayerNickname,
		WinnerState->Kills,
		WinnerState->Deaths,
		WinnerState->MatchScore));
}

void ADMPlayerController::ShowScoreboard()
{
	if (bScoreboardVisible)
	{
		return;
	}

	bScoreboardVisible = true;
	OnScoreboardVisibilityChanged(true);
}

void ADMPlayerController::HideScoreboard()
{
	if (!bScoreboardVisible)
	{
		return;
	}

	bScoreboardVisible = false;
	OnScoreboardVisibilityChanged(false);
}

void ADMPlayerController::ClientUseMatchInputMode_Implementation()
{
	bShowMouseCursor = false;
	SetIgnoreMoveInput(false);
	SetIgnoreLookInput(false);

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
}

void ADMPlayerController::ClientShowMatchHUD_Implementation()
{
	if (!IsLocalController() || MatchHUDClass == nullptr)
	{
		return;
	}

	if (MatchHUDWidget)
	{
		MatchHUDWidget->RemoveFromParent();
		MatchHUDWidget = nullptr;
	}

	MatchHUDWidget = CreateWidget<UUserWidget>(this, MatchHUDClass);

	if (MatchHUDWidget)
	{
		MatchHUDWidget->AddToViewport();
	}
}

void ADMPlayerController::ClientResubmitPlayerInfoForMatch_Implementation()
{
	if (IsLocalController())
	{
		SubmitLocalPlayerInfo();
	}
}

void ADMPlayerController::ClientApplyBlindness_Implementation(float Duration)
{
	bIsBlinded = true;
	ShowBlindnessWidget();
	OnBlindnessStarted(Duration);

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(BlindnessTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(BlindnessTimerHandle, this, &ADMPlayerController::EndBlindness, FMath::Max(0.05f, Duration), false);
	}
}

void ADMPlayerController::ClientShowEndScreen_Implementation()
{
	if (!IsLocalController())
	{
		return;
	}

	bEndScreenVisible = true;

	if (EndScreenClass && EndScreenWidget == nullptr)
	{
		EndScreenWidget = CreateWidget<UUserWidget>(this, EndScreenClass);
	}

	if (EndScreenWidget && !EndScreenWidget->IsInViewport())
	{
		EndScreenWidget->AddToViewport(EndScreenZOrder);
	}

	bShowMouseCursor = true;

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);

	OnEndScreenVisibilityChanged(true, DidLocalPlayerWin());
}

void ADMPlayerController::ClientHideEndScreen_Implementation()
{
	if (!IsLocalController())
	{
		return;
	}

	bEndScreenVisible = false;

	if (EndScreenWidget)
	{
		EndScreenWidget->RemoveFromParent();
	}

	bShowMouseCursor = false;

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);

	OnEndScreenVisibilityChanged(false, false);
}

void ADMPlayerController::EndBlindness()
{
	if (!bIsBlinded)
	{
		return;
	}

	bIsBlinded = false;
	HideBlindnessWidget();
	OnBlindnessEnded();
}

void ADMPlayerController::ShowBlindnessWidget()
{
	if (!IsLocalController() || BlindnessWidgetClass == nullptr)
	{
		return;
	}

	if (BlindnessWidget == nullptr)
	{
		BlindnessWidget = CreateWidget<UUserWidget>(this, BlindnessWidgetClass);
	}

	if (BlindnessWidget && !BlindnessWidget->IsInViewport())
	{
		BlindnessWidget->AddToViewport(BlindnessWidgetZOrder);
	}
}

void ADMPlayerController::HideBlindnessWidget()
{
	if (BlindnessWidget)
	{
		BlindnessWidget->RemoveFromParent();
	}
}

void ADMPlayerController::StartLobbyMatch(const FString& MapName)
{
	SubmitLocalPlayerInfo();
	ServerStartLobbyMatch(MapName);
}

void ADMPlayerController::ServerStartLobbyMatch_Implementation(const FString& MapName)
{
	if (!HasAuthority())
	{
		return;
	}

	ADMLobbyGameMode* LobbyGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ADMLobbyGameMode>() : nullptr;

	if (LobbyGameMode)
	{
		LobbyGameMode->StartMatch(MapName);
	}
}
