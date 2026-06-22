// Fill out your copyright notice in the Description page of Project Settings.

#include "DMCharacterPreviewActor.h"

#include "Animation/AnimInstance.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Materials/MaterialInterface.h"

ADMCharacterPreviewActor::ADMCharacterPreviewActor()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	PreviewMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PreviewMesh"));
	PreviewMesh->SetupAttachment(SceneRoot);
	PreviewMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PreviewMesh->SetGenerateOverlapEvents(false);
	PreviewMesh->SetCanEverAffectNavigation(false);
}

void ADMCharacterPreviewActor::BeginPlay()
{
	Super::BeginPlay();

	SetPreviewClass(InitialCharacterClass);
}

void ADMCharacterPreviewActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bRotatePreview)
	{
		AddActorWorldRotation(FRotator(0.0f, RotationSpeed * DeltaSeconds, 0.0f));
	}
}

void ADMCharacterPreviewActor::SetPreviewClass(EDMCharacterClass CharacterClass)
{
	SelectedPreviewClass = CharacterClass;

	switch (CharacterClass)
	{
	case EDMCharacterClass::Ash:
		ApplyPreviewData(AshPreview);
		break;
	case EDMCharacterClass::Sprit:
	default:
		ApplyPreviewData(SpritPreview);
		break;
	}

	OnPreviewClassChanged(CharacterClass);
}

void ADMCharacterPreviewActor::ApplyPreviewData(const FDMCharacterPreviewData& PreviewData)
{
	if (PreviewMesh == nullptr)
	{
		return;
	}

	PreviewMesh->SetSkeletalMesh(PreviewData.Mesh);
	PreviewMesh->SetAnimInstanceClass(PreviewData.AnimClass);
	PreviewMesh->SetRelativeLocation(PreviewData.RelativeLocation);
	PreviewMesh->SetRelativeRotation(PreviewData.RelativeRotation);
	PreviewMesh->SetRelativeScale3D(PreviewData.RelativeScale);

	for (int32 MaterialIndex = 0; MaterialIndex < PreviewData.Materials.Num(); ++MaterialIndex)
	{
		PreviewMesh->SetMaterial(MaterialIndex, PreviewData.Materials[MaterialIndex]);
	}
}
