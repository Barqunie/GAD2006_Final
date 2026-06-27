// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DMBaseTrap.generated.h"

class ADMBaseCharacter;
class APlayerState;
class UBoxComponent;
class USceneComponent;
class UStaticMeshComponent;
class UPrimitiveComponent;

UCLASS()
class AGD_API ADMBaseTrap : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADMBaseTrap();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "DM|Trap")
	void SetPlacedBy(ADMBaseCharacter* NewOwnerCharacter);

	UFUNCTION(BlueprintCallable, Category = "DM|Trap")
	void ApplyTrapEffect(ADMBaseCharacter* Character);

	UFUNCTION(BlueprintImplementableEvent, Category = "DM|Trap")
	void OnTrapTriggered(ADMBaseCharacter* Character);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Trap")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Trap")
	TObjectPtr<UStaticMeshComponent> TrapMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Trap")
	TObjectPtr<UBoxComponent> TriggerBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Trap")
	FText TrapDisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Trap")
	float Damage = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Trap")
	bool bAppliesSlow = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Trap")
	bool bAppliesBlindness = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Trap")
	float SlowMultiplier = 0.45f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Trap")
	float EffectDuration = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Trap")
	float CooldownPerPlayer = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Trap")
	float ActivationDelay = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Trap")
	float LifeTime = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Trap")
	bool bDestroyAfterTrigger = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Trap")
	bool bCanTriggerOwner = false;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	TMap<TWeakObjectPtr<ADMBaseCharacter>, float> LastTriggerTimes;

	UPROPERTY(ReplicatedUsing = OnRep_PlacedBy)
	TObjectPtr<ADMBaseCharacter> PlacedByCharacter;

	UPROPERTY(ReplicatedUsing = OnRep_PlacedBy)
	TObjectPtr<APlayerState> PlacedByPlayerState;

	float SpawnWorldTime = 0.f;

	UFUNCTION()
	void OnRep_PlacedBy();

	bool WasPlacedBy(const ADMBaseCharacter* Character) const;
	void UpdateLocalVisibility();
	bool ShouldShowTrapForLocalPlayer() const;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastTrapTriggered(ADMBaseCharacter* Character);

};
