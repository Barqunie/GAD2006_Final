// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DMBaseCharacter.h"
#include "DMAshCharacter.generated.h"

UCLASS()
class AGD_API ADMAshCharacter : public ADMBaseCharacter
{
	GENERATED_BODY()

public:
	ADMAshCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void ExecuteSkillQ() override;
	virtual void ExecuteSkillE() override;
	virtual float GetSkillQCooldown() const override;
	virtual float GetSkillECooldown() const override;

	void EndInvisibility();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Skills|Ash")
	float InvisibilityDuration = 4.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Skills|Ash")
	float InvisibilityCooldown = 8.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Skills|Ash")
	float FocusDamageMultiplier = 1.35f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Skills|Ash")
	float FocusDuration = 4.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Skills|Ash")
	float FocusCooldown = 7.f;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "DM|Skills|Ash|Runtime")
	bool bInvisible = false;

	FTimerHandle InvisibilityTimerHandle;
};
