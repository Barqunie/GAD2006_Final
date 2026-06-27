// Fill out your copyright notice in the Description page of Project Settings.

#include "DMDamageTextActor.h"

#include "Components/SceneComponent.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"

ADMDamageTextActor::ADMDamageTextActor()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Text = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Text"));
	Text->SetupAttachment(Root);
	Text->SetHorizontalAlignment(EHTA_Center);
	Text->SetVerticalAlignment(EVRTA_TextCenter);
	Text->SetWorldSize(WorldSize);
	Text->SetTextRenderColor(DamageColor);
	Text->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ADMDamageTextActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddActorWorldOffset(FVector(0.f, 0.f, FloatSpeed * DeltaTime));

	const APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	if (CameraManager == nullptr || Text == nullptr)
	{
		return;
	}

	const FVector ToCamera = CameraManager->GetCameraLocation() - GetActorLocation();
	if (!ToCamera.IsNearlyZero())
	{
		SetActorRotation(ToCamera.Rotation());
	}
}

void ADMDamageTextActor::SetDamageAmount(float DamageAmount, bool bFatalHit)
{
	if (Text)
	{
		Text->SetWorldSize(WorldSize);
		Text->SetTextRenderColor(bFatalHit ? FatalDamageColor : DamageColor);
		Text->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), FMath::Max(0.f, DamageAmount))));
	}

	SetLifeSpan(FMath::Max(0.05f, LifeTime));
}
