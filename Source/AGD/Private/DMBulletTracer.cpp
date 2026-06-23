// Fill out your copyright notice in the Description page of Project Settings.

#include "DMBulletTracer.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

ADMBulletTracer::ADMBulletTracer()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	TracerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TracerMesh"));
	TracerMesh->SetupAttachment(Root);
	TracerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TracerMesh->SetGenerateOverlapEvents(false);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		TracerMesh->SetStaticMesh(CubeMesh.Object);
	}
}

void ADMBulletTracer::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	ApplyTrace();
}

void ADMBulletTracer::BeginPlay()
{
	Super::BeginPlay();

	ApplyTrace();
	SetLifeSpan(TracerLifeTime);
}

void ADMBulletTracer::SetTrace(FVector NewTraceStart, FVector NewTraceEnd)
{
	TraceStart = NewTraceStart;
	TraceEnd = NewTraceEnd;
	ApplyTrace();
}

void ADMBulletTracer::ApplyTrace()
{
	if (TracerMesh == nullptr)
	{
		return;
	}

	const FVector TraceDelta = TraceEnd - TraceStart;
	const float TraceLength = TraceDelta.Size();

	if (TraceLength <= KINDA_SMALL_NUMBER)
	{
		TracerMesh->SetVisibility(false);
		return;
	}

	TracerMesh->SetVisibility(true);

	const FVector TraceDirection = TraceDelta / TraceLength;
	SetActorLocation(TraceStart + TraceDelta * 0.5f);
	SetActorRotation(TraceDirection.Rotation());

	TracerMesh->SetRelativeLocation(FVector::ZeroVector);
	TracerMesh->SetRelativeRotation(FRotator::ZeroRotator);
	TracerMesh->SetRelativeScale3D(FVector(TraceLength / 100.0f, Thickness, Thickness));

	if (TracerMaterial)
	{
		TracerMesh->SetMaterial(0, TracerMaterial);
	}
}
