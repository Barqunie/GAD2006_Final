// Fill out your copyright notice in the Description page of Project Settings.

#include "DMSpritCharacter.h"

ADMSpritCharacter::ADMSpritCharacter()
{
	WalkSpeed = 470.f;
	RunSpeed = 760.f;
}

void ADMSpritCharacter::ExecuteSkillQ()
{
	Dash(DashStrength);
	OnSkillUsed(TEXT("SpritDash"));
}

void ADMSpritCharacter::ExecuteSkillE()
{
	ApplySpeedBoost(SprintMultiplier, SprintDuration);
	OnSkillUsed(TEXT("SpritSprint"));
}

float ADMSpritCharacter::GetSkillQCooldown() const
{
	return DashCooldown;
}

float ADMSpritCharacter::GetSkillECooldown() const
{
	return SprintCooldown;
}
