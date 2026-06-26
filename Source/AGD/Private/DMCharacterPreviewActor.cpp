// Fill out your copyright notice in the Description page of Project Settings.

#include "DMCharacterPreviewActor.h"

#include "Animation/AnimInstance.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

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
	PreviewMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	PreviewMesh->SetUpdateAnimationInEditor(true);

	auto CreateModularMeshComponent = [this](const TCHAR* ComponentName)
	{
		USkeletalMeshComponent* ModularComponent = CreateDefaultSubobject<USkeletalMeshComponent>(ComponentName);
		ModularComponent->SetupAttachment(PreviewMesh);
		ModularComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ModularComponent->SetGenerateOverlapEvents(false);
		ModularComponent->SetCanEverAffectNavigation(false);
		ModularComponent->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
		ModularComponent->SetUpdateAnimationInEditor(true);
		ModularComponent->bUseAttachParentBound = true;
		ModularComponent->SetLeaderPoseComponent(PreviewMesh, true, true);
		return ModularComponent;
	};

	BaseTorso = CreateModularMeshComponent(TEXT("Base_Torso"));
	BaseFeet = CreateModularMeshComponent(TEXT("Base_Feet"));
	BaseHands = CreateModularMeshComponent(TEXT("Base_Hands"));
	BaseHead = CreateModularMeshComponent(TEXT("Base_Head"));
	BaseEyes = CreateModularMeshComponent(TEXT("Base_Eyes"));
	BaseTeeth = CreateModularMeshComponent(TEXT("Base_Teeth"));
	HairstyleF = CreateModularMeshComponent(TEXT("Hairstyle_F"));
	BaseLegs = CreateModularMeshComponent(TEXT("Base_Legs"));
	OutfitLower = CreateModularMeshComponent(TEXT("Outfit_Lower"));
	OutfitShoes = CreateModularMeshComponent(TEXT("Outfit_Shoes"));
	OutfitUpper = CreateModularMeshComponent(TEXT("Outfit_Upper"));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> FemaleBodyCombinedMesh(TEXT("/Game/CoreC/02BaseBody/SKM/SKM_CoreC_F_Base_Body_Combined.SKM_CoreC_F_Base_Body_Combined"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> FemaleTorsoMesh(TEXT("/Game/CoreC/02BaseBody/SKM/SKM_CoreC_F_Base_Torso.SKM_CoreC_F_Base_Torso"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> FemaleFeetMesh(TEXT("/Game/CoreC/02BaseBody/SKM/SKM_CoreC_F_Base_Feet.SKM_CoreC_F_Base_Feet"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> FemaleHandsMesh(TEXT("/Game/CoreC/02BaseBody/SKM/SKM_CoreC_F_Base_Hands.SKM_CoreC_F_Base_Hands"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> FemaleHeadMesh(TEXT("/Game/CoreC/02BaseBody/SKM/SKM_CoreC_F_Base_Face.SKM_CoreC_F_Base_Face"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> FemaleEyesMesh(TEXT("/Game/CoreC/02BaseBody/SKM/SKM_CoreC_F_Base_Eyes.SKM_CoreC_F_Base_Eyes"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> FemaleTeethMesh(TEXT("/Game/CoreC/02BaseBody/SKM/SKM_CoreC_F_Base_Teeth.SKM_CoreC_F_Base_Teeth"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> FemaleHairMesh(TEXT("/Game/CoreC/04Hairstyle/Female/SKM_CoreC_Female_HairA05.SKM_CoreC_Female_HairA05"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> FemaleLegsMesh(TEXT("/Game/CoreC/02BaseBody/SKM/SKM_CoreC_F_Base_Legs.SKM_CoreC_F_Base_Legs"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> FemaleOutfitLowerMesh(TEXT("/Game/CoreC/03Outfit/000_TechwearRunnerCourier01/SKM/SKM_CoreC_F_Techwear01_Lower.SKM_CoreC_F_Techwear01_Lower"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> FemaleOutfitShoesMesh(TEXT("/Game/CoreC/03Outfit/000_TechwearRunnerCourier01/SKM/SKM_CoreC_F_Techwear01_Shoes.SKM_CoreC_F_Techwear01_Shoes"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> FemaleOutfitUpperMesh(TEXT("/Game/CoreC/03Outfit/000_TechwearRunnerCourier01/SKM/SKM_CoreC_F_Techwear01_Upper.SKM_CoreC_F_Techwear01_Upper"));

	SpritPreview.Mesh = FemaleBodyCombinedMesh.Object;
	SpritPreview.ModularMeshes.Torso = FemaleTorsoMesh.Object;
	SpritPreview.ModularMeshes.Feet = FemaleFeetMesh.Object;
	SpritPreview.ModularMeshes.Hands = FemaleHandsMesh.Object;
	SpritPreview.ModularMeshes.Head = FemaleHeadMesh.Object;
	SpritPreview.ModularMeshes.Eyes = FemaleEyesMesh.Object;
	SpritPreview.ModularMeshes.Teeth = FemaleTeethMesh.Object;
	SpritPreview.ModularMeshes.Hair = FemaleHairMesh.Object;
	SpritPreview.ModularMeshes.Legs = FemaleLegsMesh.Object;
	SpritPreview.ModularMeshes.OutfitLower = FemaleOutfitLowerMesh.Object;
	SpritPreview.ModularMeshes.OutfitShoes = FemaleOutfitShoesMesh.Object;
	SpritPreview.ModularMeshes.OutfitUpper = FemaleOutfitUpperMesh.Object;
	SpritPreview.OutfitPresets.Add({ FemaleOutfitLowerMesh.Object, FemaleOutfitShoesMesh.Object, FemaleOutfitUpperMesh.Object });

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MaleBodyCombinedMesh(TEXT("/Game/CoreC/02BaseBody/SKM/SKM_CoreC_M_Base_Body_Combined.SKM_CoreC_M_Base_Body_Combined"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MaleTorsoMesh(TEXT("/Game/CoreC/02BaseBody/SKM/SKM_CoreC_M_Base_Torso.SKM_CoreC_M_Base_Torso"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MaleFeetMesh(TEXT("/Game/CoreC/02BaseBody/SKM/SKM_CoreC_M_Base_Feet.SKM_CoreC_M_Base_Feet"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MaleHandsMesh(TEXT("/Game/CoreC/02BaseBody/SKM/SKM_CoreC_M_Base_Hands.SKM_CoreC_M_Base_Hands"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MaleHeadMesh(TEXT("/Game/CoreC/02BaseBody/SKM/SKM_CoreC_M_Base_Face.SKM_CoreC_M_Base_Face"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MaleEyesMesh(TEXT("/Game/CoreC/02BaseBody/SKM/SKM_CoreC_M_Base_Eyes.SKM_CoreC_M_Base_Eyes"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MaleTeethMesh(TEXT("/Game/CoreC/02BaseBody/SKM/SKM_CoreC_M_Base_Teeth.SKM_CoreC_M_Base_Teeth"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MaleHairMesh(TEXT("/Game/CoreC/04Hairstyle/Male/SKM_CoreC_Male_HairA05.SKM_CoreC_Male_HairA05"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MaleLegsMesh(TEXT("/Game/CoreC/02BaseBody/SKM/SKM_CoreC_M_Base_Legs.SKM_CoreC_M_Base_Legs"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MaleOutfitLowerMesh(TEXT("/Game/CoreC/03Outfit/000_TechwearRunnerCourier01/SKM/SKM_CoreC_M_Techwear01_Lower.SKM_CoreC_M_Techwear01_Lower"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MaleOutfitShoesMesh(TEXT("/Game/CoreC/03Outfit/000_TechwearRunnerCourier01/SKM/SKM_CoreC_M_Techwear01_Shoes.SKM_CoreC_M_Techwear01_Shoes"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MaleOutfitUpperMesh(TEXT("/Game/CoreC/03Outfit/000_TechwearRunnerCourier01/SKM/SKM_CoreC_M_Techwear01_Upper.SKM_CoreC_M_Techwear01_Upper"));

	AshPreview.Mesh = MaleBodyCombinedMesh.Object;
	AshPreview.ModularMeshes.Torso = MaleTorsoMesh.Object;
	AshPreview.ModularMeshes.Feet = MaleFeetMesh.Object;
	AshPreview.ModularMeshes.Hands = MaleHandsMesh.Object;
	AshPreview.ModularMeshes.Head = MaleHeadMesh.Object;
	AshPreview.ModularMeshes.Eyes = MaleEyesMesh.Object;
	AshPreview.ModularMeshes.Teeth = MaleTeethMesh.Object;
	AshPreview.ModularMeshes.Hair = MaleHairMesh.Object;
	AshPreview.ModularMeshes.Legs = MaleLegsMesh.Object;
	AshPreview.ModularMeshes.OutfitLower = MaleOutfitLowerMesh.Object;
	AshPreview.ModularMeshes.OutfitShoes = MaleOutfitShoesMesh.Object;
	AshPreview.ModularMeshes.OutfitUpper = MaleOutfitUpperMesh.Object;
	AshPreview.OutfitPresets.Add({ MaleOutfitLowerMesh.Object, MaleOutfitShoesMesh.Object, MaleOutfitUpperMesh.Object });
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

	ApplyPreviewOutfitParts(GetSelectedPreviewData(), SelectedPreviewOutfitLowerIndex, SelectedPreviewOutfitShoesIndex, SelectedPreviewOutfitUpperIndex);
	OnPreviewClassChanged(CharacterClass);
}

void ADMCharacterPreviewActor::SetPreviewMeshes(const FDMModularCharacterMeshes& NewMeshes)
{
	SetPreviewMeshPart(EDMCharacterMeshPart::Torso, NewMeshes.Torso);
	SetPreviewMeshPart(EDMCharacterMeshPart::Feet, NewMeshes.Feet);
	SetPreviewMeshPart(EDMCharacterMeshPart::Hands, NewMeshes.Hands);
	SetPreviewMeshPart(EDMCharacterMeshPart::Head, NewMeshes.Head);
	SetPreviewMeshPart(EDMCharacterMeshPart::Eyes, NewMeshes.Eyes);
	SetPreviewMeshPart(EDMCharacterMeshPart::Teeth, NewMeshes.Teeth);
	SetPreviewMeshPart(EDMCharacterMeshPart::Hair, NewMeshes.Hair);
	SetPreviewMeshPart(EDMCharacterMeshPart::Legs, NewMeshes.Legs);
	SetPreviewMeshPart(EDMCharacterMeshPart::OutfitLower, NewMeshes.OutfitLower);
	SetPreviewMeshPart(EDMCharacterMeshPart::OutfitShoes, NewMeshes.OutfitShoes);
	SetPreviewMeshPart(EDMCharacterMeshPart::OutfitUpper, NewMeshes.OutfitUpper);
}

void ADMCharacterPreviewActor::SetPreviewMeshPart(EDMCharacterMeshPart MeshPart, USkeletalMesh* NewMesh)
{
	USkeletalMeshComponent* MeshComponent = GetPreviewMeshComponent(MeshPart);
	if (MeshComponent == nullptr)
	{
		return;
	}

	MeshComponent->SetSkeletalMesh(NewMesh);
	MeshComponent->SetLeaderPoseComponent(PreviewMesh, true, true);
	MeshComponent->RefreshBoneTransforms();
}

void ADMCharacterPreviewActor::SetPreviewOutfitIndex(int32 OutfitIndex)
{
	ApplyPreviewOutfit(GetSelectedPreviewData(), OutfitIndex);
}

int32 ADMCharacterPreviewActor::StepPreviewOutfit(int32 Direction)
{
	const int32 OutfitCount = GetPreviewOutfitCount();
	if (OutfitCount <= 0)
	{
		SelectedPreviewOutfitIndex = 0;
		return SelectedPreviewOutfitIndex;
	}

	const int32 WrappedIndex = (SelectedPreviewOutfitIndex + Direction + OutfitCount) % OutfitCount;
	SetPreviewOutfitIndex(WrappedIndex);
	return SelectedPreviewOutfitIndex;
}

void ADMCharacterPreviewActor::SetPreviewOutfitPartIndex(EDMCharacterMeshPart MeshPart, int32 PartIndex)
{
	ApplyPreviewOutfitPart(GetSelectedPreviewData(), MeshPart, PartIndex);
}

int32 ADMCharacterPreviewActor::StepPreviewOutfitPart(EDMCharacterMeshPart MeshPart, int32 Direction)
{
	const int32 OutfitCount = GetPreviewOutfitCount();
	if (OutfitCount <= 0)
	{
		SelectedPreviewOutfitLowerIndex = 0;
		SelectedPreviewOutfitShoesIndex = 0;
		SelectedPreviewOutfitUpperIndex = 0;
		return 0;
	}

	const int32 CurrentIndex = GetSelectedPreviewOutfitPartIndex(MeshPart);
	const int32 WrappedIndex = (CurrentIndex + Direction + OutfitCount) % OutfitCount;
	SetPreviewOutfitPartIndex(MeshPart, WrappedIndex);
	return WrappedIndex;
}

USkeletalMeshComponent* ADMCharacterPreviewActor::GetPreviewMeshComponent(EDMCharacterMeshPart MeshPart) const
{
	switch (MeshPart)
	{
	case EDMCharacterMeshPart::Torso:
		return BaseTorso;
	case EDMCharacterMeshPart::Feet:
		return BaseFeet;
	case EDMCharacterMeshPart::Hands:
		return BaseHands;
	case EDMCharacterMeshPart::Head:
		return BaseHead;
	case EDMCharacterMeshPart::Eyes:
		return BaseEyes;
	case EDMCharacterMeshPart::Teeth:
		return BaseTeeth;
	case EDMCharacterMeshPart::Hair:
		return HairstyleF;
	case EDMCharacterMeshPart::Legs:
		return BaseLegs;
	case EDMCharacterMeshPart::OutfitLower:
		return OutfitLower;
	case EDMCharacterMeshPart::OutfitShoes:
		return OutfitShoes;
	case EDMCharacterMeshPart::OutfitUpper:
		return OutfitUpper;
	default:
		return nullptr;
	}
}

int32 ADMCharacterPreviewActor::GetPreviewOutfitCount() const
{
	return GetSelectedPreviewData().OutfitPresets.Num();
}

int32 ADMCharacterPreviewActor::GetSelectedPreviewOutfitPartIndex(EDMCharacterMeshPart MeshPart) const
{
	switch (MeshPart)
	{
	case EDMCharacterMeshPart::OutfitLower:
		return SelectedPreviewOutfitLowerIndex;
	case EDMCharacterMeshPart::OutfitShoes:
		return SelectedPreviewOutfitShoesIndex;
	case EDMCharacterMeshPart::OutfitUpper:
		return SelectedPreviewOutfitUpperIndex;
	default:
		return 0;
	}
}

void ADMCharacterPreviewActor::ApplyPreviewData(const FDMCharacterPreviewData& PreviewData)
{
	if (PreviewMesh == nullptr)
	{
		return;
	}

	PreviewMesh->SetSkeletalMesh(PreviewData.Mesh);
	if (PreviewData.AnimClass != nullptr)
	{
		PreviewMesh->SetAnimInstanceClass(PreviewData.AnimClass);
	}
	PreviewMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	PreviewMesh->SetUpdateAnimationInEditor(true);
	PreviewMesh->SetRelativeLocation(PreviewData.RelativeLocation);
	PreviewMesh->SetRelativeRotation(PreviewData.RelativeRotation);
	PreviewMesh->SetRelativeScale3D(PreviewData.RelativeScale);

	SetPreviewMeshes(PreviewData.ModularMeshes);

	const bool bHasModularMesh =
		PreviewData.ModularMeshes.Torso != nullptr ||
		PreviewData.ModularMeshes.Feet != nullptr ||
		PreviewData.ModularMeshes.Hands != nullptr ||
		PreviewData.ModularMeshes.Head != nullptr ||
		PreviewData.ModularMeshes.Eyes != nullptr ||
		PreviewData.ModularMeshes.Teeth != nullptr ||
		PreviewData.ModularMeshes.Hair != nullptr ||
		PreviewData.ModularMeshes.Legs != nullptr ||
		PreviewData.ModularMeshes.OutfitLower != nullptr ||
		PreviewData.ModularMeshes.OutfitShoes != nullptr ||
		PreviewData.ModularMeshes.OutfitUpper != nullptr;

	PreviewMesh->SetVisibility(!bHasModularMesh, false);

	for (int32 MaterialIndex = 0; MaterialIndex < PreviewData.Materials.Num(); ++MaterialIndex)
	{
		PreviewMesh->SetMaterial(MaterialIndex, PreviewData.Materials[MaterialIndex]);
	}
}

void ADMCharacterPreviewActor::ApplyPreviewOutfit(const FDMCharacterPreviewData& PreviewData, int32 OutfitIndex)
{
	if (PreviewData.OutfitPresets.Num() <= 0)
	{
		SelectedPreviewOutfitIndex = 0;
		return;
	}

	SelectedPreviewOutfitIndex = FMath::Clamp(OutfitIndex, 0, PreviewData.OutfitPresets.Num() - 1);
	ApplyPreviewOutfitParts(PreviewData, SelectedPreviewOutfitIndex, SelectedPreviewOutfitIndex, SelectedPreviewOutfitIndex);
}

void ADMCharacterPreviewActor::ApplyPreviewOutfitPart(const FDMCharacterPreviewData& PreviewData, EDMCharacterMeshPart MeshPart, int32 PartIndex)
{
	if (PreviewData.OutfitPresets.Num() <= 0)
	{
		return;
	}

	const int32 SafePartIndex = FMath::Clamp(PartIndex, 0, PreviewData.OutfitPresets.Num() - 1);
	const FDMOutfitMeshes& Outfit = PreviewData.OutfitPresets[SafePartIndex];

	switch (MeshPart)
	{
	case EDMCharacterMeshPart::OutfitLower:
		SelectedPreviewOutfitLowerIndex = SafePartIndex;
		SetPreviewMeshPart(EDMCharacterMeshPart::OutfitLower, Outfit.Lower);
		break;
	case EDMCharacterMeshPart::OutfitShoes:
		SelectedPreviewOutfitShoesIndex = SafePartIndex;
		SetPreviewMeshPart(EDMCharacterMeshPart::OutfitShoes, Outfit.Shoes);
		break;
	case EDMCharacterMeshPart::OutfitUpper:
		SelectedPreviewOutfitUpperIndex = SafePartIndex;
		SetPreviewMeshPart(EDMCharacterMeshPart::OutfitUpper, Outfit.Upper);
		break;
	default:
		break;
	}
}

void ADMCharacterPreviewActor::ApplyPreviewOutfitParts(const FDMCharacterPreviewData& PreviewData, int32 LowerIndex, int32 ShoesIndex, int32 UpperIndex)
{
	if (PreviewData.OutfitPresets.Num() <= 0)
	{
		SelectedPreviewOutfitLowerIndex = 0;
		SelectedPreviewOutfitShoesIndex = 0;
		SelectedPreviewOutfitUpperIndex = 0;
		return;
	}

	SelectedPreviewOutfitLowerIndex = FMath::Clamp(LowerIndex, 0, PreviewData.OutfitPresets.Num() - 1);
	SelectedPreviewOutfitShoesIndex = FMath::Clamp(ShoesIndex, 0, PreviewData.OutfitPresets.Num() - 1);
	SelectedPreviewOutfitUpperIndex = FMath::Clamp(UpperIndex, 0, PreviewData.OutfitPresets.Num() - 1);

	SetPreviewMeshPart(EDMCharacterMeshPart::OutfitLower, PreviewData.OutfitPresets[SelectedPreviewOutfitLowerIndex].Lower);
	SetPreviewMeshPart(EDMCharacterMeshPart::OutfitShoes, PreviewData.OutfitPresets[SelectedPreviewOutfitShoesIndex].Shoes);
	SetPreviewMeshPart(EDMCharacterMeshPart::OutfitUpper, PreviewData.OutfitPresets[SelectedPreviewOutfitUpperIndex].Upper);
}

const FDMCharacterPreviewData& ADMCharacterPreviewActor::GetSelectedPreviewData() const
{
	return SelectedPreviewClass == EDMCharacterClass::Ash ? AshPreview : SpritPreview;
}
