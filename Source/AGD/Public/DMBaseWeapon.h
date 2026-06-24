// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DMBaseWeapon.generated.h"

class ADMBaseCharacter;
class ADMBulletTracer;
class USceneComponent;
class UStaticMeshComponent;

UCLASS()
class AGD_API ADMBaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADMBaseWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "DM|Weapon")
	void SetOwningCharacter(ADMBaseCharacter* NewOwner);

	UFUNCTION(BlueprintCallable, Category = "DM|Weapon")
	void StartFire(ADMBaseCharacter* Shooter);

	UFUNCTION(BlueprintCallable, Category = "DM|Weapon")
	void StopFire();

	UFUNCTION(BlueprintCallable, Category = "DM|Weapon")
	void Reload();

	UFUNCTION(BlueprintCallable, Category = "DM|Weapon")
	void AddReserveAmmo(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "DM|Weapon")
	bool CanFire() const;

	UFUNCTION(BlueprintImplementableEvent, Category = "DM|Weapon")
	void OnWeaponFired();

	UFUNCTION(BlueprintImplementableEvent, Category = "DM|Weapon")
	void OnShotTrace(FVector TraceStart, FVector TraceEnd, bool bHit);

	UFUNCTION(BlueprintImplementableEvent, Category = "DM|Weapon")
	void OnShotImpact(FVector ImpactPoint, FVector ImpactNormal, bool bHitCharacter);

	UFUNCTION(BlueprintImplementableEvent, Category = "DM|Weapon")
	void OnReloadStarted();

	UFUNCTION(BlueprintImplementableEvent, Category = "DM|Weapon")
	void OnReloadFinished();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Weapon")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DM|Weapon")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Weapon|Stats")
	float Damage = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Weapon|Stats")
	float Range = 10000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Weapon|Stats")
	float FireRate = 0.12f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Weapon|Stats")
	float ReloadTime = 1.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Weapon|Stats")
	float Recoil = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Weapon|Stats")
	float SpreadDegrees = 0.75f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Weapon|Stats")
	int32 Pellets = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Weapon|Stats")
	int32 MagazineSize = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Weapon|Stats")
	int32 StartingReserveAmmo = 90;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Weapon|Stats")
	bool bAutomatic = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Weapon|Visual")
	FName MuzzleSocketName = TEXT("Muzzle");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Weapon|Visual")
	FName LeftHandIKSocketName = TEXT("LeftHandIK");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Weapon|Visual")
	float MuzzleTraceOffset = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Weapon|Visual")
	TSubclassOf<ADMBulletTracer> TracerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Weapon|Stats")
	bool bUseCameraAim = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Weapon|Stats")
	bool bTraceFromCameraCenter = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DM|Weapon|Debug")
	bool bDrawDebugTrace = false;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "DM|Weapon")
	int32 CurrentAmmo = 30;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "DM|Weapon")
	int32 ReserveAmmo = 90;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "DM|Weapon")
	bool bReloading = false;

protected:
	void FireOnce();
	void FinishReload();
	FVector GetCameraAimStart() const;
	FVector GetCameraAimDirection() const;
	FVector GetCameraAimTarget() const;
	FVector GetMuzzleStart() const;
	FVector GetMuzzleDirection() const;
	bool TraceShot(const FVector& Start, const FVector& Direction, float DamageAmount, FVector& OutTraceEnd, FVector& OutImpactNormal, bool& bOutHitCharacter);
	void AddWeaponTraceIgnoredActors(FCollisionQueryParams& Params) const;
	bool FindFirstValidTraceHit(const FVector& Start, const FVector& End, FHitResult& OutHit) const;
	bool ShouldIgnoreTraceActor(const AActor* Actor) const;
	void SpawnTracer(FVector TraceStart, FVector TraceEnd);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastWeaponFired(FVector TraceStart, FVector TraceEnd, bool bHit, FVector ImpactNormal, bool bHitCharacter);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastReloadStarted();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastReloadFinished();

	UPROPERTY(Replicated)
	TObjectPtr<ADMBaseCharacter> OwningCharacter;

	FTimerHandle AutoFireTimerHandle;
	FTimerHandle ReloadTimerHandle;
	float LastFireTime = -100.f;

};
