// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DMDamageTextActor.generated.h"

class USceneComponent;
class UTextRenderComponent;

UCLASS(Blueprintable)
class AGD_API ADMDamageTextActor : public AActor
{
	GENERATED_BODY()

public:
	ADMDamageTextActor();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "DM|Damage Text")
	void SetDamageAmount(float DamageAmount, bool bFatalHit = false);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Damage Text")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Damage Text")
	TObjectPtr<UTextRenderComponent> Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Damage Text")
	float LifeTime = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Damage Text")
	float FloatSpeed = 85.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Damage Text")
	float WorldSize = 42.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Damage Text")
	FColor DamageColor = FColor(255, 80, 40);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Damage Text")
	FColor FatalDamageColor = FColor(255, 0, 0);
};
