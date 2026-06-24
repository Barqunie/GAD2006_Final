// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DMTypes.h"
#include "DMCharacterPreviewActor.generated.h"

class UAnimInstance;
class UMaterialInterface;
class USceneComponent;
class USkeletalMesh;
class USkeletalMeshComponent;

USTRUCT(BlueprintType)
struct FDMCharacterPreviewData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Preview")
	TObjectPtr<USkeletalMesh> Mesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Preview")
	FDMModularCharacterMeshes ModularMeshes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Preview")
	TSubclassOf<UAnimInstance> AnimClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Preview")
	TArray<TObjectPtr<UMaterialInterface>> Materials;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Preview")
	FVector RelativeLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Preview")
	FRotator RelativeRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Preview")
	FVector RelativeScale = FVector::OneVector;
};

UCLASS()
class AGD_API ADMCharacterPreviewActor : public AActor
{
	GENERATED_BODY()

public:
	ADMCharacterPreviewActor();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "DM|Preview")
	void SetPreviewClass(EDMCharacterClass CharacterClass);

	UFUNCTION(BlueprintCallable, Category = "DM|Preview")
	void SetPreviewMeshes(const FDMModularCharacterMeshes& NewMeshes);

	UFUNCTION(BlueprintCallable, Category = "DM|Preview")
	void SetPreviewMeshPart(EDMCharacterMeshPart MeshPart, USkeletalMesh* NewMesh);

	UFUNCTION(BlueprintPure, Category = "DM|Preview")
	USkeletalMeshComponent* GetPreviewMeshComponent(EDMCharacterMeshPart MeshPart) const;

	UFUNCTION(BlueprintPure, Category = "DM|Preview")
	EDMCharacterClass GetSelectedPreviewClass() const { return SelectedPreviewClass; }

	UFUNCTION(BlueprintImplementableEvent, Category = "DM|Preview")
	void OnPreviewClassChanged(EDMCharacterClass CharacterClass);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Preview")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Preview")
	TObjectPtr<USkeletalMeshComponent> PreviewMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Preview|Modular")
	TObjectPtr<USkeletalMeshComponent> BaseTorso;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Preview|Modular")
	TObjectPtr<USkeletalMeshComponent> BaseFeet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Preview|Modular")
	TObjectPtr<USkeletalMeshComponent> BaseHands;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Preview|Modular")
	TObjectPtr<USkeletalMeshComponent> BaseHead;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Preview|Modular")
	TObjectPtr<USkeletalMeshComponent> BaseEyes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Preview|Modular")
	TObjectPtr<USkeletalMeshComponent> BaseTeeth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Preview|Modular")
	TObjectPtr<USkeletalMeshComponent> HairstyleF;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Preview|Modular")
	TObjectPtr<USkeletalMeshComponent> BaseLegs;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Preview|Modular")
	TObjectPtr<USkeletalMeshComponent> OutfitLower;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Preview|Modular")
	TObjectPtr<USkeletalMeshComponent> OutfitShoes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Preview|Modular")
	TObjectPtr<USkeletalMeshComponent> OutfitUpper;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Preview")
	EDMCharacterClass InitialCharacterClass = EDMCharacterClass::Sprit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Preview")
	FDMCharacterPreviewData SpritPreview;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Preview")
	FDMCharacterPreviewData AshPreview;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Preview")
	bool bRotatePreview = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Preview", meta = (EditCondition = "bRotatePreview"))
	float RotationSpeed = 18.0f;

private:
	void ApplyPreviewData(const FDMCharacterPreviewData& PreviewData);

	EDMCharacterClass SelectedPreviewClass = EDMCharacterClass::Sprit;
};
