// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DMBasePickup.generated.h"

class ADMBaseCharacter;
class ADMBaseTrap;
class UBoxComponent;
class USphereComponent;
class USceneComponent;
class UStaticMeshComponent;
class UTextRenderComponent;
class UPrimitiveComponent;

UENUM(BlueprintType)
enum class EDMPickupType : uint8
{
	Health,
	Ammo,
	SpeedBoost,
	DamageBoost,
	Trap
};

USTRUCT(BlueprintType)
struct FDMTrapPickupOption
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Pickup|Trap")
	TSubclassOf<ADMBaseTrap> TrapClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Pickup|Trap")
	FText DisplayName;
};

UCLASS()
class AGD_API ADMBasePickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADMBasePickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UFUNCTION(BlueprintCallable, Category = "DM|Pickup")
	virtual void ApplyPickup(ADMBaseCharacter* Character);

	UFUNCTION(BlueprintCallable, Category = "DM|Pickup|Prompt")
	void RefreshPickupPromptText();

	UFUNCTION(BlueprintImplementableEvent, Category = "DM|Pickup")
	void OnPickedUp(ADMBaseCharacter* Character);

	UFUNCTION(BlueprintImplementableEvent, Category = "DM|Pickup")
	void OnRespawned();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Pickup")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Pickup")
	TObjectPtr<UStaticMeshComponent> PickupMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Pickup")
	TObjectPtr<USphereComponent> Collision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Pickup|Prompt")
	TObjectPtr<UBoxComponent> PromptBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Pickup|Prompt")
	TObjectPtr<UTextRenderComponent> PickupPromptText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Pickup|Prompt")
	bool bShowPickupPrompt = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Pickup")
	EDMPickupType PickupType = EDMPickupType::Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Pickup")
	float Amount = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Pickup")
	float Duration = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Pickup")
	float RespawnTime = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Pickup")
	bool bRespawns = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Pickup|Trap")
	TArray<TSubclassOf<ADMBaseTrap>> TrapClasses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Pickup|Trap")
	TArray<FDMTrapPickupOption> TrapOptions;

protected:
	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnPromptOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnPromptOverlapEnd(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	void SetPickupActive(bool bNewActive);
	void SetPromptVisible(bool bVisible);
	FText GetPickupPromptText() const;
	void RespawnPickup();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPickedUp(ADMBaseCharacter* Character);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRespawned();

	FTimerHandle RespawnTimerHandle;
	bool bActive = true;

};
