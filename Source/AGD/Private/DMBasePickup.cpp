// Fill out your copyright notice in the Description page of Project Settings.


#include "DMBasePickup.h"

#include "DMBaseCharacter.h"
#include "DMBaseTrap.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

// Sets default values
ADMBasePickup::ADMBasePickup()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	PickupMesh->SetupAttachment(Root);

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->SetupAttachment(Root);
	Collision->SetSphereRadius(80.f);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
	Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	PromptBox = CreateDefaultSubobject<UBoxComponent>(TEXT("PromptBox"));
	PromptBox->SetupAttachment(Root);
	PromptBox->SetBoxExtent(FVector(225.f, 225.f, 150.f));
	PromptBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PromptBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	PromptBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	PickupPromptText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("PickupPromptText"));
	PickupPromptText->SetupAttachment(Root);
	PickupPromptText->SetText(FText::FromString(TEXT("Pick Up")));
	PickupPromptText->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	PickupPromptText->SetHorizontalAlignment(EHTA_Center);
	PickupPromptText->SetVerticalAlignment(EVRTA_TextCenter);
	PickupPromptText->SetWorldSize(32.f);
	PickupPromptText->SetHiddenInGame(true);
}

// Called when the game starts or when spawned
void ADMBasePickup::BeginPlay()
{
	Super::BeginPlay();

	RefreshPickupPromptText();

	if (Collision)
	{
		Collision->OnComponentBeginOverlap.AddDynamic(this, &ADMBasePickup::OnOverlapBegin);
	}

	if (PromptBox)
	{
		PromptBox->OnComponentBeginOverlap.AddDynamic(this, &ADMBasePickup::OnPromptOverlapBegin);
		PromptBox->OnComponentEndOverlap.AddDynamic(this, &ADMBasePickup::OnPromptOverlapEnd);
	}

	SetPromptVisible(false);
	
}

void ADMBasePickup::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopPromptFacingUpdates();

	Super::EndPlay(EndPlayReason);
}

void ADMBasePickup::RefreshPickupPromptText()
{
	if (PickupPromptText)
	{
		PickupPromptText->SetText(GetPickupPromptText());
	}
}

void ADMBasePickup::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!HasAuthority() || !bActive)
	{
		return;
	}

	ADMBaseCharacter* Character = Cast<ADMBaseCharacter>(OtherActor);

	if (Character == nullptr)
	{
		return;
	}

	ApplyPickup(Character);
	MulticastPickedUp(Character);
	SetPickupActive(false);

	if (bRespawns && GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &ADMBasePickup::RespawnPickup, RespawnTime, false);
	}
}

void ADMBasePickup::ApplyPickup(ADMBaseCharacter* Character)
{
	if (Character == nullptr)
	{
		return;
	}

	switch (PickupType)
	{
	case EDMPickupType::Health:
		Character->Heal(Amount);
		break;

	case EDMPickupType::Ammo:
		Character->AddAmmo(FMath::RoundToInt(Amount));
		break;

	case EDMPickupType::SpeedBoost:
		Character->ApplySpeedBoost(Amount, Duration);
		break;

	case EDMPickupType::DamageBoost:
		Character->ApplyDamageBoost(Amount, Duration);
		break;

	case EDMPickupType::Trap:
		{
			TArray<FDMTrapPickupOption> ValidTrapOptions;

			for (const FDMTrapPickupOption& TrapOption : TrapOptions)
			{
				if (TrapOption.TrapClass)
				{
					ValidTrapOptions.Add(TrapOption);
				}
			}

			if (ValidTrapOptions.Num() > 0)
			{
				const int32 RandomIndex = FMath::RandRange(0, ValidTrapOptions.Num() - 1);
				const FDMTrapPickupOption& SelectedOption = ValidTrapOptions[RandomIndex];
				Character->SetAvailableTrap(SelectedOption.TrapClass, SelectedOption.DisplayName);
				break;
			}

			TArray<TSubclassOf<ADMBaseTrap>> ValidTrapClasses;

			for (TSubclassOf<ADMBaseTrap> TrapClass : TrapClasses)
			{
				if (TrapClass)
				{
					ValidTrapClasses.Add(TrapClass);
				}
			}

			if (ValidTrapClasses.Num() > 0)
			{
				const int32 RandomIndex = FMath::RandRange(0, ValidTrapClasses.Num() - 1);
				Character->SetAvailableTrapClass(ValidTrapClasses[RandomIndex]);
			}
		}
		break;

	default:
		break;
	}
}

void ADMBasePickup::OnPromptOverlapBegin(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!bActive || !bShowPickupPrompt)
	{
		return;
	}

	ADMBaseCharacter* Character = Cast<ADMBaseCharacter>(OtherActor);
	if (Character == nullptr || !Character->IsLocallyControlled())
	{
		return;
	}

	SetPromptVisible(true);
}

void ADMBasePickup::OnPromptOverlapEnd(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
)
{
	ADMBaseCharacter* Character = Cast<ADMBaseCharacter>(OtherActor);
	if (Character == nullptr || !Character->IsLocallyControlled())
	{
		return;
	}

	SetPromptVisible(false);
}

void ADMBasePickup::SetPickupActive(bool bNewActive)
{
	bActive = bNewActive;
	SetActorHiddenInGame(!bActive);
	SetActorEnableCollision(bActive);
	SetPromptVisible(false);
}

void ADMBasePickup::SetPromptVisible(bool bVisible)
{
	if (PickupPromptText)
	{
		PickupPromptText->SetHiddenInGame(!bVisible);
	}

	if (bVisible)
	{
		UpdatePromptFacingCamera();
		StartPromptFacingUpdates();
	}
	else
	{
		StopPromptFacingUpdates();
	}
}

void ADMBasePickup::StartPromptFacingUpdates()
{
	if (UWorld* World = GetWorld())
	{
		const float UpdateInterval = FMath::Max(0.02f, PromptFacingUpdateInterval);
		World->GetTimerManager().SetTimer(
			PromptFacingTimerHandle,
			this,
			&ADMBasePickup::UpdatePromptFacingCamera,
			UpdateInterval,
			true
		);
	}
}

void ADMBasePickup::StopPromptFacingUpdates()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(PromptFacingTimerHandle);
	}
}

void ADMBasePickup::UpdatePromptFacingCamera()
{
	if (PickupPromptText == nullptr || PickupPromptText->bHiddenInGame)
	{
		return;
	}

	const APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	if (CameraManager == nullptr)
	{
		return;
	}

	const FVector ToCamera = CameraManager->GetCameraLocation() - PickupPromptText->GetComponentLocation();
	if (ToCamera.IsNearlyZero())
	{
		return;
	}

	PickupPromptText->SetWorldRotation(ToCamera.Rotation() + PromptTextRotationOffset);
}

FText ADMBasePickup::GetPickupPromptText() const
{
	switch (PickupType)
	{
	case EDMPickupType::Health:
		return FText::FromString(TEXT("Pick Up Health"));
	case EDMPickupType::Ammo:
		return FText::FromString(TEXT("Pick Up Ammo"));
	case EDMPickupType::SpeedBoost:
		return FText::FromString(TEXT("Pick Up Speed Boost"));
	case EDMPickupType::DamageBoost:
		return FText::FromString(TEXT("Pick Up Damage Boost"));
	case EDMPickupType::Trap:
		return FText::FromString(TEXT("Pick Up Trap"));
	default:
		return FText::FromString(TEXT("Pick Up"));
	}
}

void ADMBasePickup::RespawnPickup()
{
	SetPickupActive(true);
	MulticastRespawned();
}

void ADMBasePickup::MulticastPickedUp_Implementation(ADMBaseCharacter* Character)
{
	SetPromptVisible(false);
	OnPickedUp(Character);
}

void ADMBasePickup::MulticastRespawned_Implementation()
{
	SetPromptVisible(false);
	OnRespawned();
}


