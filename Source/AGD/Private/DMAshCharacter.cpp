// Fill out your copyright notice in the Description page of Project Settings.

#include "DMAshCharacter.h"

#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

ADMAshCharacter::ADMAshCharacter()
{
	WalkSpeed = 450.f;
	RunSpeed = 720.f;
}

void ADMAshCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADMAshCharacter, bInvisible);
}

void ADMAshCharacter::ExecuteSkillQ()
{
	bInvisible = true;
	SetCharacterVisibilityForSkill(false);

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			InvisibilityTimerHandle,
			this,
			&ADMAshCharacter::EndInvisibility,
			InvisibilityDuration,
			false
		);
	}

	OnSkillUsed(TEXT("AshInvisibility"));
}

void ADMAshCharacter::ExecuteSkillE()
{
	ApplyDamageBoost(FocusDamageMultiplier, FocusDuration);
	OnSkillUsed(TEXT("AshFocus"));
}

float ADMAshCharacter::GetSkillQCooldown() const
{
	return InvisibilityCooldown;
}

float ADMAshCharacter::GetSkillECooldown() const
{
	return FocusCooldown;
}

void ADMAshCharacter::EndInvisibility()
{
	bInvisible = false;
	SetCharacterVisibilityForSkill(true);
}
