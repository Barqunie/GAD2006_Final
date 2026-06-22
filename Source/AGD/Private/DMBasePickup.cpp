// Fill out your copyright notice in the Description page of Project Settings.


#include "DMBasePickup.h"

#include "DMBaseCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
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

}

// Called when the game starts or when spawned
void ADMBasePickup::BeginPlay()
{
	Super::BeginPlay();

	if (Collision)
	{
		Collision->OnComponentBeginOverlap.AddDynamic(this, &ADMBasePickup::OnOverlapBegin);
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

void ADMBasePickup::SetPickupActive(bool bNewActive)
{
	bActive = bNewActive;
	SetActorHiddenInGame(!bActive);
	SetActorEnableCollision(bActive);
}

void ADMBasePickup::RespawnPickup()
{
	SetPickupActive(true);
	MulticastRespawned();
}

void ADMBasePickup::MulticastPickedUp_Implementation(ADMBaseCharacter* Character)
{
	OnPickedUp(Character);
}

void ADMBasePickup::MulticastRespawned_Implementation()
{
	OnRespawned();
}


