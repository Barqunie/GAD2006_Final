// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DMTypes.h"
#include "DMPlayerState.h"
#include "DMBaseCharacter.generated.h"

class ADMBaseWeapon;
class ADMBaseTrap;
class UAnimMontage;
class UCameraComponent;
class USkeletalMesh;
class USkeletalMeshComponent;
class USpringArmComponent;

UCLASS()
class AGD_API ADMBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADMBaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "DM|Combat")
	void TakeWeaponDamage(float DamageAmount, ADMPlayerState* AttackerState);

	UFUNCTION(BlueprintCallable, Category = "DM|Combat")
	void Heal(float HealAmount);

	UFUNCTION(BlueprintCallable, Category = "DM|Weapon")
	void AddAmmo(int32 AmmoAmount);

	UFUNCTION(BlueprintCallable, Category = "DM|Weapon")
	void HandleReloadStarted();

	UFUNCTION(BlueprintCallable, Category = "DM|Weapon")
	void HandleReloadFinished();

	UFUNCTION(BlueprintCallable, Category = "DM|Trap")
	bool PlaceTrap();

	UFUNCTION(BlueprintCallable, Category = "DM|Trap")
	void SetAvailableTrapClass(TSubclassOf<ADMBaseTrap> NewTrapClass);

	UFUNCTION(BlueprintCallable, Category = "DM|Trap")
	void SetAvailableTrap(TSubclassOf<ADMBaseTrap> NewTrapClass, FText DisplayName);

	UFUNCTION(BlueprintPure, Category = "DM|Trap")
	bool HasAvailableTrap() const;

	UFUNCTION(BlueprintPure, Category = "DM|Trap")
	FText GetAvailableTrapDisplayName() const;

	UFUNCTION(BlueprintPure, Category = "DM|Weapon|IK")
	bool GetWeaponLeftHandIKTransform(FVector& OutLocation, FRotator& OutRotation) const;

	UFUNCTION(BlueprintCallable, Category = "DM|Aim")
	bool GetCrosshairAimPoint(FVector& OutAimPoint) const;

	UFUNCTION(BlueprintPure, Category = "DM|Aim")
	FVector GetCurrentAimPoint() const { return CurrentAimPoint; }

	UFUNCTION(BlueprintPure, Category = "DM|Aim")
	float GetCurrentAimPitch() const { return CurrentAimPitch; }

	UFUNCTION(BlueprintCallable, Category = "DM|Effects")
	void ApplySpeedBoost(float Multiplier, float Duration);

	UFUNCTION(BlueprintCallable, Category = "DM|Effects")
	void ApplySlow(float Multiplier, float Duration);

	UFUNCTION(BlueprintCallable, Category = "DM|Effects")
	void ApplyDamageBoost(float Multiplier, float Duration);

	UFUNCTION(BlueprintCallable, Category = "DM|Effects")
	void ApplyBlindness(float Duration);

	UFUNCTION(BlueprintCallable, Category = "DM|Character")
	void ResetAfterSpawn();

	UFUNCTION(BlueprintCallable, Category = "DM|Character|Modular")
	void SetModularCharacterMeshes(const FDMModularCharacterMeshes& NewMeshes);

	UFUNCTION(BlueprintCallable, Category = "DM|Character|Modular")
	void SetModularMeshPart(EDMCharacterMeshPart MeshPart, USkeletalMesh* NewMesh);

	UFUNCTION(BlueprintCallable, Category = "DM|Character|Outfit")
	void ApplyOutfitByIndex(int32 OutfitIndex);

	UFUNCTION(BlueprintCallable, Category = "DM|Character|Outfit")
	void ApplyPlayerCustomizationFromPlayerState();

	UFUNCTION(BlueprintPure, Category = "DM|Character|Outfit")
	int32 GetSelectedOutfitIndex() const { return SelectedOutfitIndex; }

	UFUNCTION(BlueprintPure, Category = "DM|Character|Outfit")
	int32 GetOutfitCount() const { return OutfitPresets.Num(); }

	UFUNCTION(BlueprintPure, Category = "DM|Character|Modular")
	USkeletalMeshComponent* GetModularMeshComponent(EDMCharacterMeshPart MeshPart) const;

	UFUNCTION(BlueprintImplementableEvent, Category = "DM|Combat")
	void OnHealthChanged();

	UFUNCTION(BlueprintImplementableEvent, Category = "DM|Combat")
	void OnDeath();

	UFUNCTION(BlueprintImplementableEvent, Category = "DM|Weapon")
	void OnWeaponChanged();

	UFUNCTION(BlueprintImplementableEvent, Category = "DM|Skills")
	void OnSkillUsed(FName SkillName);

	UFUNCTION(BlueprintImplementableEvent, Category = "DM|Effects")
	void OnSpeedBoostStarted(float SpeedMultiplier, float JumpMultiplier, float Duration);

	UFUNCTION(BlueprintImplementableEvent, Category = "DM|Effects")
	void OnSpeedBoostEnded();

	UFUNCTION(BlueprintImplementableEvent, Category = "DM|Effects")
	void OnDamageBoostStarted(float DamageMultiplierValue, float Duration);

	UFUNCTION(BlueprintImplementableEvent, Category = "DM|Effects")
	void OnDamageBoostEnded();

	UFUNCTION(BlueprintImplementableEvent, Category = "DM|Movement")
	void OnSlideStarted();

	UFUNCTION(BlueprintImplementableEvent, Category = "DM|Movement")
	void OnSlideEnded();

	UFUNCTION(BlueprintImplementableEvent, Category = "DM|Trap")
	void OnTrapPlaced(ADMBaseTrap* PlacedTrap);

	UFUNCTION(BlueprintImplementableEvent, Category = "DM|Trap")
	void OnTrapClassChanged(TSubclassOf<ADMBaseTrap> NewTrapClass);

	UFUNCTION(BlueprintImplementableEvent, Category = "DM|Character|Modular")
	void OnModularCharacterMeshesChanged();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Camera")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Camera")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Character|Modular")
	TObjectPtr<USkeletalMeshComponent> BaseTorso;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Character|Modular")
	TObjectPtr<USkeletalMeshComponent> BaseFeet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Character|Modular")
	TObjectPtr<USkeletalMeshComponent> BaseHands;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Character|Modular")
	TObjectPtr<USkeletalMeshComponent> BaseHead;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Character|Modular")
	TObjectPtr<USkeletalMeshComponent> BaseEyes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Character|Modular")
	TObjectPtr<USkeletalMeshComponent> BaseTeeth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Character|Modular")
	TObjectPtr<USkeletalMeshComponent> HairstyleF;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Character|Modular")
	TObjectPtr<USkeletalMeshComponent> BaseLegs;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Character|Modular")
	TObjectPtr<USkeletalMeshComponent> OutfitLower;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Character|Modular")
	TObjectPtr<USkeletalMeshComponent> OutfitShoes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Character|Modular")
	TObjectPtr<USkeletalMeshComponent> OutfitUpper;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Character|Modular")
	FDMModularCharacterMeshes DefaultModularMeshes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Character|Outfit")
	TArray<FDMOutfitMeshes> OutfitPresets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Character|Modular")
	bool bHideMainMeshWhenUsingModularMeshes = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DM|Weapon")
	TSubclassOf<ADMBaseWeapon> DefaultWeaponClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, ReplicatedUsing = OnRep_AvailableTrapClass, Category = "DM|Trap")
	TSubclassOf<ADMBaseTrap> DefaultTrapClass;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AvailableTrapClass, Category = "DM|Trap")
	FString AvailableTrapDisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Trap")
	float TrapPlacementDistance = 180.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Trap")
	float TrapPlacementTraceHeight = 160.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Trap")
	float TrapPlacementTraceDepth = 450.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Trap")
	float TrapPlacementCooldown = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Trap")
	int32 MaxPlacedTraps = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Weapon|Animation")
	TObjectPtr<UAnimMontage> ReloadMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Weapon|Animation")
	bool bLockMovementDuringReload = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Movement|Slide")
	TObjectPtr<UAnimMontage> SlideMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Movement|Slide")
	float SlideDuration = 0.65f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Movement|Slide")
	float SlideCooldown = 0.85f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Movement|Slide")
	float MinSlideSpeed = 550.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Movement|Slide")
	float SlideStartSpeed = 1250.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Movement|Slide")
	float SlideEndSpeed = 760.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Movement|Slide")
	float SlideForwardBias = 0.65f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Movement|Slide")
	float SlideGroundFriction = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Movement|Slide")
	float SlideBrakingDeceleration = 320.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Movement|Slide")
	bool bCanFireWhileSliding = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Movement|Slide")
	bool bCanAimWhileSliding = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Weapon|IK")
	FName LeftHandIKTargetBone = TEXT("hand_r");

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "DM|Weapon|Runtime")
	TObjectPtr<ADMBaseWeapon> CurrentWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Combat")
	float MaxHealth = 100.f;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "DM|Combat|Runtime")
	float Health = 100.f;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "DM|Combat|Runtime")
	bool bIsDead = false;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "DM|Combat|Runtime")
	float DamageMultiplier = 1.0f;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Aiming, Category = "DM|Combat|Runtime")
	bool bIsAiming = false;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Running, Category = "DM|Movement|Runtime")
	bool bIsRunning = false;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_SkillHidden, Category = "DM|Effects|Runtime")
	bool bSkillHidden = false;

	UPROPERTY(BlueprintReadOnly, Category = "DM|Weapon|Runtime")
	bool bIsReloading = false;

	UPROPERTY(BlueprintReadOnly, Category = "DM|Movement|Runtime")
	bool bIsSliding = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Movement")
	float WalkSpeed = 450.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Movement")
	float RunSpeed = 700.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Effects")
	bool bRagdollOnDeath = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Effects")
	float SpeedBoostJumpMultiplier = 1.35f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Camera")
	float NormalCameraArmLength = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Camera")
	float AimCameraArmLength = 180.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Camera")
	FVector NormalCameraSocketOffset = FVector(0.f, 60.f, 45.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Camera")
	FVector AimCameraSocketOffset = FVector(0.f, 85.f, 35.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Camera")
	float NormalCameraFOV = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Camera")
	float AimCameraFOV = 65.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Camera")
	float CameraAimInterpSpeed = 12.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Aim")
	bool bUseCrosshairAimRotation = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Aim")
	float AimTraceDistance = 50000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Aim")
	float AimRotationInterpSpeed = 18.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Aim")
	float AimRotationReplicationInterval = 0.04f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Aim")
	float AimRotationYawOffset = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Aim")
	bool bDrawAimDebug = false;

	UPROPERTY(BlueprintReadOnly, Category = "DM|Aim")
	FVector CurrentAimPoint = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "DM|Aim")
	float CurrentAimPitch = 0.f;

protected:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void UpdateRotationToCrosshair(float DeltaTime);
	void UpdateAimAnimationValues();
	void StartRunning();
	void StopRunning();
	void StartFire();
	void StopFire();
	void StartAiming();
	void StopAiming();
	void Reload();
	void StartSlide();
	void TryPlaceTrap();
	void UseSkillQ();
	void UseSkillE();
	void Dash(float Strength);
	void EndSlide();
	void EndTemporaryMovementEffect();
	void EndDamageBoost();

	UFUNCTION()
	void OnRep_Health();

	UFUNCTION()
	void OnRep_Aiming();

	UFUNCTION()
	void OnRep_Running();

	UFUNCTION()
	void OnRep_SkillHidden();

	UFUNCTION()
	void OnRep_AvailableTrapClass();

	UFUNCTION()
	void OnRep_SelectedOutfitIndex();

	UFUNCTION(Server, Reliable)
	void ServerSetRunning(bool bNewRunning);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bNewAiming);

	UFUNCTION(Server, Unreliable)
	void ServerSetAimRotation(FRotator NewRotation);

	UFUNCTION(Server, Reliable)
	void ServerStartFire();

	UFUNCTION(Server, Reliable)
	void ServerStopFire();

	UFUNCTION(Server, Reliable)
	void ServerReload();

	UFUNCTION(Server, Reliable)
	void ServerStartSlide();

	UFUNCTION(Server, Reliable)
	void ServerPlaceTrap();

	UFUNCTION(Server, Reliable)
	void ServerUseSkillQ();

	UFUNCTION(Server, Reliable)
	void ServerUseSkillE();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastHandleDeath();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpeedBoostStarted(float SpeedMultiplier, float JumpMultiplier, float Duration);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpeedBoostEnded();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastDamageBoostStarted(float DamageMultiplierValue, float Duration);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastDamageBoostEnded();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSlideStarted(FVector_NetQuantizeNormal SlideDirection);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastApplyOutfitIndex(int32 OutfitIndex);

	void SpawnDefaultWeapon();
	bool CanUseSkill(float LastUseTime, float Cooldown) const;
	bool CanPlaceTrap() const;
	FString ResolveTrapDisplayName(TSubclassOf<ADMBaseTrap> TrapClass) const;
	bool FindTrapPlacementTransform(FTransform& OutTransform) const;
	void RegisterPlacedTrap(ADMBaseTrap* Trap);
	bool CanStartSlide() const;
	FVector GetSlideDirection() const;
	void BeginSlide(const FVector& SlideDirection);
	void UpdateSlideMovement(float DeltaTime);
	void ApplyDeathRagdoll();
	void ApplyAimState(bool bNewAiming);
	void ApplyRunningState(bool bNewRunning);
	void UpdateCameraSettings(float DeltaTime);
	void ApplySkillVisibility();
	void SetCharacterVisibilityForSkill(bool bVisible);
	virtual void ExecuteSkillQ();
	virtual void ExecuteSkillE();
	virtual float GetSkillQCooldown() const;
	virtual float GetSkillECooldown() const;
	virtual float ModifyIncomingDamage(float DamageAmount) const;

	FTimerHandle MovementEffectTimerHandle;
	FTimerHandle DamageBoostTimerHandle;
	FTimerHandle SlideTimerHandle;

	float TemporaryMovementMultiplier = 1.0f;
	float CachedBaseJumpZVelocity = 420.f;
	float CachedGroundFriction = 8.0f;
	float CachedBrakingDecelerationWalking = 2048.0f;
	FVector CurrentSlideDirection = FVector::ForwardVector;
	float SlideStartWorldTime = 0.f;
	bool bSpeedBoostActive = false;
	bool bJumpBoostActive = false;
	float LastSlideTime = -100.f;
	float LastTrapPlacementTime = -100.f;
	float LastAimRotationReplicationTime = -100.f;
	TArray<TWeakObjectPtr<ADMBaseTrap>> PlacedTraps;

	float LastSkillQTime = -100.f;
	float LastSkillETime = -100.f;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_SelectedOutfitIndex, Category = "DM|Character|Outfit")
	int32 SelectedOutfitIndex = 0;
};
