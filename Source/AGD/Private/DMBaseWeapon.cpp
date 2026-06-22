// Fill out your copyright notice in the Description page of Project Settings.


#include "DMBaseWeapon.h"

#include "DMBaseCharacter.h"
#include "DMPlayerState.h"
#include "Camera/CameraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

// Sets default values
ADMBaseWeapon::ADMBaseWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(Root);

}

// Called when the game starts or when spawned
void ADMBaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		CurrentAmmo = MagazineSize;
		ReserveAmmo = StartingReserveAmmo;
	}
}

void ADMBaseWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADMBaseWeapon, CurrentAmmo);
	DOREPLIFETIME(ADMBaseWeapon, ReserveAmmo);
	DOREPLIFETIME(ADMBaseWeapon, bReloading);
	DOREPLIFETIME(ADMBaseWeapon, OwningCharacter);
}

void ADMBaseWeapon::SetOwningCharacter(ADMBaseCharacter* NewOwner)
{
	OwningCharacter = NewOwner;
	SetOwner(NewOwner);
}

void ADMBaseWeapon::StartFire(ADMBaseCharacter* Shooter)
{
	if (!HasAuthority())
	{
		return;
	}

	if (Shooter)
	{
		SetOwningCharacter(Shooter);
	}

	FireOnce();

	if (bAutomatic && GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(AutoFireTimerHandle, this, &ADMBaseWeapon::FireOnce, FireRate, true);
	}
}

void ADMBaseWeapon::StopFire()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(AutoFireTimerHandle);
	}
}

bool ADMBaseWeapon::CanFire() const
{
	const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	return HasAuthority()
		&& OwningCharacter != nullptr
		&& !bReloading
		&& CurrentAmmo > 0
		&& Now - LastFireTime >= FireRate;
}

void ADMBaseWeapon::FireOnce()
{
	if (!CanFire())
	{
		if (CurrentAmmo <= 0)
		{
			Reload();
		}

		return;
	}

	LastFireTime = GetWorld()->GetTimeSeconds();
	CurrentAmmo--;

	const bool bShouldTraceFromCamera = bTraceFromCameraCenter && OwningCharacter && OwningCharacter->bIsAiming;
	const FVector RawStart = bShouldTraceFromCamera ? GetCameraAimStart() : GetMuzzleStart();
	const FVector Direction = bShouldTraceFromCamera
		? GetCameraAimDirection()
		: bUseCameraAim
			? (GetCameraAimTarget() - RawStart).GetSafeNormal()
			: GetMuzzleDirection();
	const FVector Start = bShouldTraceFromCamera ? RawStart : RawStart + Direction * MuzzleTraceOffset;
	const float FinalDamage = Damage * (OwningCharacter ? OwningCharacter->DamageMultiplier : 1.0f);

	for (int32 PelletIndex = 0; PelletIndex < FMath::Max(1, Pellets); PelletIndex++)
	{
		const FVector ShotDirection = FMath::VRandCone(Direction, FMath::DegreesToRadians(SpreadDegrees));
		FVector TraceEnd;
		const bool bHit = TraceShot(Start, ShotDirection, FinalDamage, TraceEnd);
		MulticastWeaponFired(Start, TraceEnd, bHit);
	}
}

FVector ADMBaseWeapon::GetCameraAimStart() const
{
	if (OwningCharacter && OwningCharacter->Camera)
	{
		return OwningCharacter->Camera->GetComponentLocation();
	}

	return OwningCharacter ? OwningCharacter->GetActorLocation() + FVector(0.f, 0.f, 50.f) : GetActorLocation();
}

FVector ADMBaseWeapon::GetCameraAimDirection() const
{
	if (OwningCharacter && OwningCharacter->Camera)
	{
		return OwningCharacter->Camera->GetForwardVector();
	}

	return OwningCharacter ? OwningCharacter->GetActorForwardVector() : GetActorForwardVector();
}

FVector ADMBaseWeapon::GetCameraAimTarget() const
{
	if (GetWorld() == nullptr)
	{
		return GetActorLocation() + GetActorForwardVector() * Range;
	}

	const FVector Start = GetCameraAimStart();
	const FVector End = Start + GetCameraAimDirection().GetSafeNormal() * Range;

	FHitResult Hit;
	const bool bHit = FindFirstValidTraceHit(Start, End, Hit);
	return bHit ? Hit.ImpactPoint : End;
}

FVector ADMBaseWeapon::GetMuzzleStart() const
{
	if (WeaponMesh && WeaponMesh->DoesSocketExist(MuzzleSocketName))
	{
		return WeaponMesh->GetSocketLocation(MuzzleSocketName);
	}

	if (WeaponMesh)
	{
		return WeaponMesh->GetComponentLocation();
	}

	return GetActorLocation();
}

FVector ADMBaseWeapon::GetMuzzleDirection() const
{
	if (WeaponMesh && WeaponMesh->DoesSocketExist(MuzzleSocketName))
	{
		return WeaponMesh->GetSocketRotation(MuzzleSocketName).Vector().GetSafeNormal();
	}

	if (WeaponMesh)
	{
		return WeaponMesh->GetForwardVector().GetSafeNormal();
	}

	return GetActorForwardVector().GetSafeNormal();
}

void ADMBaseWeapon::AddWeaponTraceIgnoredActors(FCollisionQueryParams& Params) const
{
	Params.bIgnoreTouches = true;
	Params.bFindInitialOverlaps = false;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());

	if (WeaponMesh)
	{
		Params.AddIgnoredComponent(WeaponMesh.Get());
	}

	if (OwningCharacter)
	{
		Params.AddIgnoredActor(OwningCharacter);

		TArray<UPrimitiveComponent*> OwningComponents;
		OwningCharacter->GetComponents<UPrimitiveComponent>(OwningComponents);

		for (UPrimitiveComponent* Component : OwningComponents)
		{
			if (Component)
			{
				Params.AddIgnoredComponent(Component);
			}
		}

		TArray<AActor*> AttachedActors;
		OwningCharacter->GetAttachedActors(AttachedActors);
		Params.AddIgnoredActors(AttachedActors);
	}
}

bool ADMBaseWeapon::FindFirstValidTraceHit(const FVector& Start, const FVector& End, FHitResult& OutHit) const
{
	if (GetWorld() == nullptr)
	{
		return false;
	}

	TArray<FHitResult> Hits;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(DMWeaponTrace), true);
	AddWeaponTraceIgnoredActors(Params);

	if (!GetWorld()->LineTraceMultiByChannel(Hits, Start, End, ECC_Visibility, Params))
	{
		return false;
	}

	for (const FHitResult& Hit : Hits)
	{
		if (ShouldIgnoreTraceActor(Hit.GetActor()))
		{
			continue;
		}

		OutHit = Hit;
		return true;
	}

	return false;
}

bool ADMBaseWeapon::ShouldIgnoreTraceActor(const AActor* Actor) const
{
	if (Actor == nullptr || Actor == this || Actor == GetOwner() || Actor == OwningCharacter)
	{
		return true;
	}

	if (Actor->IsAttachedTo(this) || (OwningCharacter && Actor->IsAttachedTo(OwningCharacter)))
	{
		return true;
	}

	for (const AActor* ActorOwner = Actor->GetOwner(); ActorOwner != nullptr; ActorOwner = ActorOwner->GetOwner())
	{
		if (ActorOwner == this || ActorOwner == GetOwner() || ActorOwner == OwningCharacter)
		{
			return true;
		}
	}

	return false;
}

bool ADMBaseWeapon::TraceShot(const FVector& Start, const FVector& Direction, float DamageAmount, FVector& OutTraceEnd)
{
	if (GetWorld() == nullptr)
	{
		OutTraceEnd = Start;
		return false;
	}

	const FVector End = Start + Direction.GetSafeNormal() * Range;
	FHitResult Hit;
	const bool bHit = FindFirstValidTraceHit(Start, End, Hit);
	OutTraceEnd = bHit ? Hit.ImpactPoint : End;

	if (bDrawDebugTrace)
	{
		DrawDebugLine(GetWorld(), Start, OutTraceEnd, bHit ? FColor::Green : FColor::Red, false, 1.5f, 0, 1.5f);
	}

	if (!bHit)
	{
		return false;
	}

	ADMBaseCharacter* HitCharacter = Cast<ADMBaseCharacter>(Hit.GetActor());

	if (HitCharacter && HitCharacter != OwningCharacter)
	{
		ADMPlayerState* AttackerState = OwningCharacter ? OwningCharacter->GetPlayerState<ADMPlayerState>() : nullptr;
		HitCharacter->TakeWeaponDamage(DamageAmount, AttackerState);
	}

	return true;
}

void ADMBaseWeapon::MulticastWeaponFired_Implementation(FVector TraceStart, FVector TraceEnd, bool bHit)
{
	OnWeaponFired();
	OnShotTrace(TraceStart, TraceEnd, bHit);
}

void ADMBaseWeapon::Reload()
{
	if (!HasAuthority() || bReloading || CurrentAmmo >= MagazineSize || ReserveAmmo <= 0)
	{
		return;
	}

	StopFire();
	bReloading = true;
	MulticastReloadStarted();

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &ADMBaseWeapon::FinishReload, ReloadTime, false);
	}
}

void ADMBaseWeapon::FinishReload()
{
	const int32 NeededAmmo = MagazineSize - CurrentAmmo;
	const int32 AmmoToLoad = FMath::Min(NeededAmmo, ReserveAmmo);

	CurrentAmmo += AmmoToLoad;
	ReserveAmmo -= AmmoToLoad;
	bReloading = false;
	MulticastReloadFinished();
}

void ADMBaseWeapon::MulticastReloadStarted_Implementation()
{
	if (OwningCharacter)
	{
		OwningCharacter->HandleReloadStarted();
	}

	OnReloadStarted();
}

void ADMBaseWeapon::MulticastReloadFinished_Implementation()
{
	if (OwningCharacter)
	{
		OwningCharacter->HandleReloadFinished();
	}

	OnReloadFinished();
}

void ADMBaseWeapon::AddReserveAmmo(int32 Amount)
{
	if (!HasAuthority())
	{
		return;
	}

	ReserveAmmo += FMath::Max(0, Amount);
}

