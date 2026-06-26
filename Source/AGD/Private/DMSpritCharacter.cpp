// Fill out your copyright notice in the Description page of Project Settings.

#include "DMSpritCharacter.h"

#include "Engine/SkeletalMesh.h"
#include "UObject/ConstructorHelpers.h"

ADMSpritCharacter::ADMSpritCharacter()
{
	WalkSpeed = 470.f;
	RunSpeed = 760.f;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> BodyCombinedMesh(TEXT("/Game/CoreC/02BaseBody/SKM/SKM_CoreC_F_Base_Body_Combined.SKM_CoreC_F_Base_Body_Combined"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> TorsoMesh(TEXT("/Game/CoreC/02BaseBody/SKM/SKM_CoreC_F_Base_Torso.SKM_CoreC_F_Base_Torso"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> FeetMesh(TEXT("/Game/CoreC/02BaseBody/SKM/SKM_CoreC_F_Base_Feet.SKM_CoreC_F_Base_Feet"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> HandsMesh(TEXT("/Game/CoreC/02BaseBody/SKM/SKM_CoreC_F_Base_Hands.SKM_CoreC_F_Base_Hands"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> HeadMesh(TEXT("/Game/CoreC/02BaseBody/SKM/SKM_CoreC_F_Base_Face.SKM_CoreC_F_Base_Face"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> EyesMesh(TEXT("/Game/CoreC/02BaseBody/SKM/SKM_CoreC_F_Base_Eyes.SKM_CoreC_F_Base_Eyes"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> TeethMesh(TEXT("/Game/CoreC/02BaseBody/SKM/SKM_CoreC_F_Base_Teeth.SKM_CoreC_F_Base_Teeth"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> HairMesh(TEXT("/Game/CoreC/04Hairstyle/Female/SKM_CoreC_Female_HairA05.SKM_CoreC_Female_HairA05"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> LegsMesh(TEXT("/Game/CoreC/02BaseBody/SKM/SKM_CoreC_F_Base_Legs.SKM_CoreC_F_Base_Legs"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> OutfitLowerMesh(TEXT("/Game/CoreC/03Outfit/000_TechwearRunnerCourier01/SKM/SKM_CoreC_F_Techwear01_Lower.SKM_CoreC_F_Techwear01_Lower"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> OutfitShoesMesh(TEXT("/Game/CoreC/03Outfit/000_TechwearRunnerCourier01/SKM/SKM_CoreC_F_Techwear01_Shoes.SKM_CoreC_F_Techwear01_Shoes"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> OutfitUpperMesh(TEXT("/Game/CoreC/03Outfit/000_TechwearRunnerCourier01/SKM/SKM_CoreC_F_Techwear01_Upper.SKM_CoreC_F_Techwear01_Upper"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> OutfitLowerMesh1(TEXT("/Game/CoreC/03Outfit/000_TechwearRunnerCourier01/SKM/SKM_CoreC_F_Techwear01_Lower1.SKM_CoreC_F_Techwear01_Lower1"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> OutfitShoesMesh1(TEXT("/Game/CoreC/03Outfit/000_TechwearRunnerCourier01/SKM/SKM_CoreC_F_Techwear01_Shoes1.SKM_CoreC_F_Techwear01_Shoes1"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> OutfitUpperMesh1(TEXT("/Game/CoreC/03Outfit/000_TechwearRunnerCourier01/SKM/SKM_CoreC_F_Techwear01_Upper1.SKM_CoreC_F_Techwear01_Upper1"));

	if (BodyCombinedMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(BodyCombinedMesh.Object);
	}

	DefaultModularMeshes.Torso = TorsoMesh.Object;
	DefaultModularMeshes.Feet = FeetMesh.Object;
	DefaultModularMeshes.Hands = HandsMesh.Object;
	DefaultModularMeshes.Head = HeadMesh.Object;
	DefaultModularMeshes.Eyes = EyesMesh.Object;
	DefaultModularMeshes.Teeth = TeethMesh.Object;
	DefaultModularMeshes.Hair = HairMesh.Object;
	DefaultModularMeshes.Legs = LegsMesh.Object;
	DefaultModularMeshes.OutfitLower = OutfitLowerMesh.Object;
	DefaultModularMeshes.OutfitShoes = OutfitShoesMesh.Object;
	DefaultModularMeshes.OutfitUpper = OutfitUpperMesh.Object;

	OutfitPresets.Reset();
	OutfitPresets.Add({ OutfitLowerMesh.Object, OutfitShoesMesh.Object, OutfitUpperMesh.Object });
	OutfitPresets.Add({ OutfitLowerMesh1.Object, OutfitShoesMesh1.Object, OutfitUpperMesh1.Object });
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
