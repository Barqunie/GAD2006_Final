// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DMBulletTracer.generated.h"

class UMaterialInterface;
class USceneComponent;
class UStaticMeshComponent;

UCLASS(Blueprintable)
class AGD_API ADMBulletTracer : public AActor
{
	GENERATED_BODY()

public:
	ADMBulletTracer();

	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "DM|Tracer")
	void SetTrace(FVector NewTraceStart, FVector NewTraceEnd);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Tracer")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Tracer")
	TObjectPtr<UStaticMeshComponent> TracerMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Tracer", meta = (ExposeOnSpawn = "true"))
	FVector TraceStart = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Tracer", meta = (ExposeOnSpawn = "true"))
	FVector TraceEnd = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Tracer")
	TObjectPtr<UMaterialInterface> TracerMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Tracer", meta = (ClampMin = "0.001"))
	float Thickness = 0.025f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Tracer", meta = (ClampMin = "0.01"))
	float TracerLifeTime = 0.06f;

protected:
	void ApplyTrace();
};
