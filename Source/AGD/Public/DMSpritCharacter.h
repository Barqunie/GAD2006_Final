// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DMBaseCharacter.h"
#include "DMSpritCharacter.generated.h"

UCLASS()
class AGD_API ADMSpritCharacter : public ADMBaseCharacter
{
	GENERATED_BODY()

public:
	ADMSpritCharacter();

protected:
	virtual void ExecuteSkillQ() override;
	virtual void ExecuteSkillE() override;
	virtual float GetSkillQCooldown() const override;
	virtual float GetSkillECooldown() const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Skills|Sprit")
	float DashStrength = 650.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Skills|Sprit")
	float DashCooldown = 4.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Skills|Sprit")
	float SprintMultiplier = 1.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Skills|Sprit")
	float SprintDuration = 4.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Skills|Sprit")
	float SprintCooldown = 7.f;
};
