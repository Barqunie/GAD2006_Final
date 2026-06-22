// Fill out your copyright notice in the Description page of Project Settings.

#include "DMBaseTrap.h"

#include "DMBaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ADMBaseTrap::ADMBaseTrap()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	TrapDisplayName = NSLOCTEXT("DMTrap", "DefaultTrapDisplayName", "Trap");

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	TrapMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrapMesh"));
	TrapMesh->SetupAttachment(Root);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(Root);
	TriggerBox->SetBoxExtent(FVector(100.f, 100.f, 40.f));
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ADMBaseTrap::BeginPlay()
{
	Super::BeginPlay();

	SpawnWorldTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;

	if (LifeTime > 0.f)
	{
		SetLifeSpan(LifeTime);
	}

	if (TriggerBox)
	{
		TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ADMBaseTrap::OnOverlapBegin);
	}
}

void ADMBaseTrap::SetPlacedBy(ADMBaseCharacter* NewOwnerCharacter)
{
	PlacedByCharacter = NewOwnerCharacter;
	SetOwner(NewOwnerCharacter);
}

void ADMBaseTrap::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!HasAuthority())
	{
		return;
	}

	ADMBaseCharacter* Character = Cast<ADMBaseCharacter>(OtherActor);

	if (Character == nullptr)
	{
		return;
	}

	const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;

	if (Now - SpawnWorldTime < ActivationDelay)
	{
		return;
	}

	if (!bCanTriggerOwner && Character == PlacedByCharacter.Get())
	{
		return;
	}

	const float* LastTime = LastTriggerTimes.Find(Character);

	if (LastTime && Now - *LastTime < CooldownPerPlayer)
	{
		return;
	}

	LastTriggerTimes.Add(Character, Now);
	ApplyTrapEffect(Character);
	MulticastTrapTriggered(Character);

	if (bDestroyAfterTrigger)
	{
		SetLifeSpan(0.15f);
	}
}

void ADMBaseTrap::ApplyTrapEffect(ADMBaseCharacter* Character)
{
	if (Character == nullptr)
	{
		return;
	}

	if (Damage > 0.f)
	{
		Character->TakeWeaponDamage(Damage, nullptr);
	}

	if (bAppliesSlow)
	{
		Character->ApplySlow(SlowMultiplier, EffectDuration);
	}

	if (bAppliesBlindness)
	{
		Character->ApplyBlindness(EffectDuration);
	}
}

void ADMBaseTrap::MulticastTrapTriggered_Implementation(ADMBaseCharacter* Character)
{
	OnTrapTriggered(Character);
}
