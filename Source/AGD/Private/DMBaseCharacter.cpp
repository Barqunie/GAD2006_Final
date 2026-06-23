// Fill out your copyright notice in the Description page of Project Settings.


#include "DMBaseCharacter.h"

#include "DMBaseTrap.h"
#include "DMBaseWeapon.h"
#include "DMGameMode.h"
#include "DMPlayerController.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

// Sets default values
ADMBaseCharacter::ADMBaseCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = NormalCameraArmLength;
	SpringArm->SocketOffset = NormalCameraSocketOffset;
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;
	Camera->FieldOfView = NormalCameraFOV;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

// Called when the game starts or when spawned
void ADMBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetCharacterMovement())
	{
		CachedBaseJumpZVelocity = GetCharacterMovement()->JumpZVelocity;
	}

	ResetAfterSpawn();

	if (HasAuthority())
	{
		SpawnDefaultWeapon();
	}

}

void ADMBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADMBaseCharacter, CurrentWeapon);
	DOREPLIFETIME(ADMBaseCharacter, Health);
	DOREPLIFETIME(ADMBaseCharacter, bIsDead);
	DOREPLIFETIME(ADMBaseCharacter, DamageMultiplier);
	DOREPLIFETIME(ADMBaseCharacter, bIsAiming);
	DOREPLIFETIME(ADMBaseCharacter, bIsRunning);
	DOREPLIFETIME(ADMBaseCharacter, bSkillHidden);
	DOREPLIFETIME(ADMBaseCharacter, DefaultTrapClass);
	DOREPLIFETIME(ADMBaseCharacter, AvailableTrapDisplayName);
}

// Called every frame
void ADMBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateAimAnimationValues();
	UpdateRotationToCrosshair(DeltaTime);
	UpdateSlideMovement(DeltaTime);
	UpdateCameraSettings(DeltaTime);
}

// Called to bind functionality to input
void ADMBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ADMBaseCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ADMBaseCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ADMBaseCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ADMBaseCharacter::AddControllerPitchInput);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Pressed, this, &ADMBaseCharacter::StartRunning);
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Released, this, &ADMBaseCharacter::StopRunning);
	PlayerInputComponent->BindAction(TEXT("Aim"), IE_Pressed, this, &ADMBaseCharacter::StartAiming);
	PlayerInputComponent->BindAction(TEXT("Aim"), IE_Released, this, &ADMBaseCharacter::StopAiming);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ADMBaseCharacter::StartFire);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Released, this, &ADMBaseCharacter::StopFire);
	PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, this, &ADMBaseCharacter::Reload);
	PlayerInputComponent->BindAction(TEXT("Slide"), IE_Pressed, this, &ADMBaseCharacter::StartSlide);
	PlayerInputComponent->BindAction(TEXT("PlaceTrap"), IE_Pressed, this, &ADMBaseCharacter::TryPlaceTrap);
	PlayerInputComponent->BindAction(TEXT("SkillQ"), IE_Pressed, this, &ADMBaseCharacter::UseSkillQ);
	PlayerInputComponent->BindAction(TEXT("SkillE"), IE_Pressed, this, &ADMBaseCharacter::UseSkillE);
}

void ADMBaseCharacter::MoveForward(float Value)
{
	if (Controller == nullptr || bIsDead || FMath::IsNearlyZero(Value) || (bLockMovementDuringReload && bIsReloading))
	{
		return;
	}

	const FRotator ControlRotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, Value);
}

void ADMBaseCharacter::MoveRight(float Value)
{
	if (Controller == nullptr || bIsDead || FMath::IsNearlyZero(Value) || (bLockMovementDuringReload && bIsReloading))
	{
		return;
	}

	const FRotator ControlRotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(Direction, Value);
}

bool ADMBaseCharacter::GetCrosshairAimPoint(FVector& OutAimPoint) const
{
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return false;
	}

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC == nullptr)
	{
		return false;
	}

	FVector TraceStart = FVector::ZeroVector;
	FVector TraceDirection = FVector::ForwardVector;
	bool bHasAimRay = false;

	if (PC->IsLocalController())
	{
		int32 ViewportX = 0;
		int32 ViewportY = 0;
		PC->GetViewportSize(ViewportX, ViewportY);

		if (ViewportX > 0 && ViewportY > 0)
		{
			const float ScreenX = ViewportX * 0.5f;
			const float ScreenY = ViewportY * 0.5f;
			bHasAimRay = PC->DeprojectScreenPositionToWorld(ScreenX, ScreenY, TraceStart, TraceDirection);
		}
	}

	if (!bHasAimRay)
	{
		FRotator ViewRotation;
		PC->GetPlayerViewPoint(TraceStart, ViewRotation);
		TraceDirection = ViewRotation.Vector();
		bHasAimRay = true;
	}

	if (!bHasAimRay && Camera)
	{
		TraceStart = Camera->GetComponentLocation();
		TraceDirection = Camera->GetForwardVector();
	}
	else if (!bHasAimRay)
	{
		TraceStart = GetActorLocation() + FVector(0.f, 0.f, 50.f);
		TraceDirection = GetActorForwardVector();
	}

	TraceDirection = TraceDirection.GetSafeNormal();
	const FVector TraceEnd = TraceStart + TraceDirection * AimTraceDistance;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(DMCrosshairAimTrace), true);
	Params.bIgnoreTouches = true;
	Params.AddIgnoredActor(this);

	if (CurrentWeapon)
	{
		Params.AddIgnoredActor(CurrentWeapon);
	}

	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors);
	Params.AddIgnoredActors(AttachedActors);

	FHitResult Hit;
	const bool bHit = World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params);
	OutAimPoint = bHit ? Hit.ImpactPoint : TraceEnd;

	if (bDrawAimDebug)
	{
		DrawDebugLine(World, TraceStart, OutAimPoint, bHit ? FColor::Green : FColor::Red, false, 0.f, 0, 1.5f);
		DrawDebugSphere(World, OutAimPoint, 10.f, 12, FColor::Yellow, false, 0.f);
	}

	return true;
}

void ADMBaseCharacter::UpdateRotationToCrosshair(float DeltaTime)
{
	if (!bUseCrosshairAimRotation || bIsDead || !IsLocallyControlled())
	{
		return;
	}

	FVector AimPoint;
	if (!GetCrosshairAimPoint(AimPoint))
	{
		return;
	}

	CurrentAimPoint = AimPoint;

	FVector AimDirection = AimPoint - GetActorLocation();
	AimDirection.Z = 0.f;

	if (AimDirection.IsNearlyZero())
	{
		return;
	}

	FRotator TargetRotation = AimDirection.Rotation();
	TargetRotation.Yaw += AimRotationYawOffset;
	TargetRotation.Pitch = 0.f;
	TargetRotation.Roll = 0.f;

	const float InterpSpeed = FMath::Max(0.1f, AimRotationInterpSpeed);
	const FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, InterpSpeed);
	SetActorRotation(FRotator(0.f, NewRotation.Yaw, 0.f));
}

void ADMBaseCharacter::UpdateAimAnimationValues()
{
	CurrentAimPitch = FRotator::NormalizeAxis(GetBaseAimRotation().Pitch);
}

void ADMBaseCharacter::StartRunning()
{
	if (bIsDead || (bLockMovementDuringReload && bIsReloading))
	{
		return;
	}

	bIsRunning = true;
	bIsAiming = false;
	ApplyAimState(false);
	ApplyRunningState(true);
	StopFire();
	ServerSetRunning(true);
}

void ADMBaseCharacter::StopRunning()
{
	bIsRunning = false;
	ApplyRunningState(false);
	ServerSetRunning(false);
}

void ADMBaseCharacter::ServerSetRunning_Implementation(bool bNewRunning)
{
	bIsRunning = bNewRunning && !bIsDead;

	if (bIsRunning)
	{
		bIsAiming = false;
		ApplyAimState(false);

		if (CurrentWeapon)
		{
			CurrentWeapon->StopFire();
		}
	}

	ApplyRunningState(bIsRunning);
}

void ADMBaseCharacter::StartAiming()
{
	if (bIsDead || (bIsSliding && !bCanAimWhileSliding))
	{
		return;
	}

	if (bIsRunning && !bIsSliding)
	{
		bIsRunning = false;
		ApplyRunningState(false);
		ServerSetRunning(false);
	}

	bIsAiming = true;
	ApplyAimState(true);
	ServerSetAiming(true);
}

void ADMBaseCharacter::StopAiming()
{
	bIsAiming = false;
	ApplyAimState(false);
	ServerSetAiming(false);
}

void ADMBaseCharacter::ServerSetAiming_Implementation(bool bNewAiming)
{
	bIsAiming = bNewAiming && !bIsDead;

	if (bIsAiming)
	{
		if (!bIsSliding)
		{
			bIsRunning = false;
			ApplyRunningState(false);
		}

		if (CurrentWeapon)
		{
			CurrentWeapon->StopFire();
		}
	}

	ApplyAimState(bIsAiming);
}

void ADMBaseCharacter::StartFire()
{
	if (bIsDead || bSkillHidden || (bIsRunning && !(bIsSliding && bCanFireWhileSliding)) || bIsReloading)
	{
		return;
	}

	ServerStartFire();
}

void ADMBaseCharacter::StopFire()
{
	ServerStopFire();
}

void ADMBaseCharacter::Reload()
{
	if (bIsRunning || bIsSliding || bIsReloading)
	{
		return;
	}

	ServerReload();
}

void ADMBaseCharacter::ServerStartFire_Implementation()
{
	const bool bMovementBlocksFire = bIsRunning && !(bIsSliding && bCanFireWhileSliding);

	if (CurrentWeapon && !bIsDead && !bSkillHidden && !bMovementBlocksFire && !bIsReloading)
	{
		CurrentWeapon->StartFire(this);
	}
}

void ADMBaseCharacter::ServerStopFire_Implementation()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}

void ADMBaseCharacter::ServerReload_Implementation()
{
	if (CurrentWeapon && !bIsRunning && !bIsSliding && !bIsReloading)
	{
		CurrentWeapon->Reload();
	}
}

void ADMBaseCharacter::StartSlide()
{
	if (!CanStartSlide())
	{
		return;
	}

	ServerStartSlide();
}

void ADMBaseCharacter::ServerStartSlide_Implementation()
{
	if (!CanStartSlide())
	{
		return;
	}

	LastSlideTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	MulticastSlideStarted(GetSlideDirection());
}

void ADMBaseCharacter::TryPlaceTrap()
{
	if (bIsDead || bIsReloading || bIsSliding)
	{
		return;
	}

	ServerPlaceTrap();
}

void ADMBaseCharacter::ServerPlaceTrap_Implementation()
{
	PlaceTrap();
}

void ADMBaseCharacter::UseSkillQ()
{
	ServerUseSkillQ();
}

void ADMBaseCharacter::UseSkillE()
{
	ServerUseSkillE();
}

bool ADMBaseCharacter::CanUseSkill(float LastUseTime, float Cooldown) const
{
	const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	return !bIsDead && Now - LastUseTime >= Cooldown;
}

void ADMBaseCharacter::ServerUseSkillQ_Implementation()
{
	if (!CanUseSkill(LastSkillQTime, GetSkillQCooldown()))
	{
		return;
	}

	LastSkillQTime = GetWorld()->GetTimeSeconds();
	ExecuteSkillQ();
}

void ADMBaseCharacter::ServerUseSkillE_Implementation()
{
	if (!CanUseSkill(LastSkillETime, GetSkillECooldown()))
	{
		return;
	}

	LastSkillETime = GetWorld()->GetTimeSeconds();
	ExecuteSkillE();
}

void ADMBaseCharacter::ExecuteSkillQ()
{
	OnSkillUsed(TEXT("BaseSkillQ"));
}

void ADMBaseCharacter::ExecuteSkillE()
{
	OnSkillUsed(TEXT("BaseSkillE"));
}

float ADMBaseCharacter::GetSkillQCooldown() const
{
	return 0.f;
}

float ADMBaseCharacter::GetSkillECooldown() const
{
	return 0.f;
}

float ADMBaseCharacter::ModifyIncomingDamage(float DamageAmount) const
{
	return DamageAmount;
}

void ADMBaseCharacter::Dash(float Strength)
{
	if (GetCharacterMovement() == nullptr || bIsDead)
	{
		return;
	}

	FVector Direction = GetLastMovementInputVector();

	if (Direction.IsNearlyZero())
	{
		if (Controller)
		{
			const FRotator ControlRotation = Controller->GetControlRotation();
			const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
			Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		}
		else
		{
			Direction = GetActorForwardVector();
		}
	}

	Direction.Z = 0.f;
	Direction.Normalize();

	GetCharacterMovement()->StopMovementImmediately();

	const FVector StartLocation = GetActorLocation();
	const FVector TargetLocation = StartLocation + Direction * Strength;
	FHitResult Hit;
	SetActorLocation(TargetLocation, true, &Hit, ETeleportType::TeleportPhysics);
}

bool ADMBaseCharacter::CanStartSlide() const
{
	const UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (MovementComponent == nullptr || bIsDead || bIsSliding || bIsReloading || bIsAiming || !bIsRunning)
	{
		return false;
	}

	if (!MovementComponent->IsMovingOnGround())
	{
		return false;
	}

	const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	if (Now - LastSlideTime < SlideCooldown)
	{
		return false;
	}

	return MovementComponent->Velocity.Size2D() >= MinSlideSpeed;
}

FVector ADMBaseCharacter::GetSlideDirection() const
{
	FVector MovementDirection = GetVelocity();
	MovementDirection.Z = 0.f;

	if (MovementDirection.IsNearlyZero())
	{
		MovementDirection = GetLastMovementInputVector();
		MovementDirection.Z = 0.f;
	}

	FVector ForwardDirection = GetActorForwardVector();
	if (Controller)
	{
		const FRotator ControlRotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
		ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	}
	ForwardDirection.Z = 0.f;
	ForwardDirection.Normalize();

	if (MovementDirection.IsNearlyZero())
	{
		MovementDirection = ForwardDirection;
	}

	MovementDirection.Normalize();

	const float ForwardWeight = FMath::Clamp(SlideForwardBias, 0.f, 1.f);
	return FMath::Lerp(MovementDirection, ForwardDirection, ForwardWeight).GetSafeNormal();
}

void ADMBaseCharacter::BeginSlide(const FVector& SlideDirection)
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (MovementComponent == nullptr || bIsDead)
	{
		return;
	}

	if (!bIsSliding)
	{
		CachedGroundFriction = MovementComponent->GroundFriction;
		CachedBrakingDecelerationWalking = MovementComponent->BrakingDecelerationWalking;
	}

	bIsSliding = true;
	LastSlideTime = GetWorld() ? GetWorld()->GetTimeSeconds() : LastSlideTime;
	SlideStartWorldTime = LastSlideTime;
	ApplyAimState(bIsAiming);

	MovementComponent->GroundFriction = SlideGroundFriction;
	MovementComponent->BrakingDecelerationWalking = SlideBrakingDeceleration;

	const FVector Direction = SlideDirection.IsNearlyZero() ? GetActorForwardVector() : SlideDirection.GetSafeNormal();
	CurrentSlideDirection = Direction;
	const float CurrentSpeed = MovementComponent->Velocity.Size2D();
	FVector SlideVelocity = Direction * FMath::Max(SlideStartSpeed, CurrentSpeed);
	SlideVelocity.Z = MovementComponent->Velocity.Z;
	MovementComponent->Velocity = SlideVelocity;

	if (SlideMontage && GetMesh())
	{
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			AnimInstance->Montage_Play(SlideMontage);
		}
	}

	OnSlideStarted();

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(SlideTimerHandle, this, &ADMBaseCharacter::EndSlide, SlideDuration, false);
	}
}

void ADMBaseCharacter::UpdateSlideMovement(float DeltaTime)
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (!bIsSliding || MovementComponent == nullptr)
	{
		return;
	}

	if (!MovementComponent->IsMovingOnGround())
	{
		EndSlide();
		return;
	}

	const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : SlideStartWorldTime;
	const float SlideAlpha = SlideDuration > 0.f ? FMath::Clamp((Now - SlideStartWorldTime) / SlideDuration, 0.f, 1.f) : 1.f;
	const float TargetSpeed = FMath::Lerp(SlideStartSpeed, SlideEndSpeed, SlideAlpha);

	const FVector Direction = CurrentSlideDirection.IsNearlyZero() ? GetActorForwardVector() : CurrentSlideDirection.GetSafeNormal();
	const float CurrentSpeed = MovementComponent->Velocity.Size2D();
	const float NewSpeed = FMath::FInterpTo(CurrentSpeed, TargetSpeed, DeltaTime, 10.f);

	FVector NewVelocity = Direction * NewSpeed;
	NewVelocity.Z = MovementComponent->Velocity.Z;
	MovementComponent->Velocity = NewVelocity;
}

void ADMBaseCharacter::EndSlide()
{
	if (!bIsSliding)
	{
		return;
	}

	bIsSliding = false;

	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		MovementComponent->GroundFriction = CachedGroundFriction;
		MovementComponent->BrakingDecelerationWalking = CachedBrakingDecelerationWalking;
		ApplyRunningState(bIsRunning);
	}

	if (!CurrentSlideDirection.IsNearlyZero() && !bIsAiming)
	{
		SetActorRotation(CurrentSlideDirection.Rotation());
	}

	if (SlideMontage && GetMesh())
	{
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			if (AnimInstance->Montage_IsPlaying(SlideMontage))
			{
				AnimInstance->Montage_Stop(0.12f, SlideMontage);
			}
		}
	}

	OnSlideEnded();
}

void ADMBaseCharacter::TakeWeaponDamage(float DamageAmount, ADMPlayerState* AttackerState)
{
	if (!HasAuthority() || bIsDead)
	{
		return;
	}

	const float FinalDamage = ModifyIncomingDamage(DamageAmount);
	Health = FMath::Clamp(Health - FinalDamage, 0.f, MaxHealth);
	OnHealthChanged();

	if (Health > 0.f)
	{
		return;
	}

	bIsDead = true;
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}

	MulticastHandleDeath();

	ADMGameMode* GameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ADMGameMode>() : nullptr;
	ADMPlayerState* VictimState = GetPlayerState<ADMPlayerState>();

	if (GameMode)
	{
		GameMode->HandlePlayerKilled(AttackerState, VictimState);
	}
}

void ADMBaseCharacter::Heal(float HealAmount)
{
	if (!HasAuthority() || bIsDead)
	{
		return;
	}

	Health = FMath::Clamp(Health + HealAmount, 0.f, MaxHealth);
	OnHealthChanged();
}

void ADMBaseCharacter::AddAmmo(int32 AmmoAmount)
{
	if (CurrentWeapon)
	{
		CurrentWeapon->AddReserveAmmo(AmmoAmount);
	}
}

bool ADMBaseCharacter::CanPlaceTrap() const
{
	const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;

	return DefaultTrapClass != nullptr
		&& !bIsDead
		&& !bIsReloading
		&& !bIsSliding
		&& Now - LastTrapPlacementTime >= TrapPlacementCooldown;
}

void ADMBaseCharacter::SetAvailableTrapClass(TSubclassOf<ADMBaseTrap> NewTrapClass)
{
	SetAvailableTrap(NewTrapClass, FText::GetEmpty());
}

void ADMBaseCharacter::SetAvailableTrap(TSubclassOf<ADMBaseTrap> NewTrapClass, FText DisplayName)
{
	DefaultTrapClass = NewTrapClass;
	AvailableTrapDisplayName = DisplayName.ToString().IsEmpty() ? ResolveTrapDisplayName(NewTrapClass) : DisplayName.ToString();
	OnTrapClassChanged(DefaultTrapClass);
}

bool ADMBaseCharacter::HasAvailableTrap() const
{
	return DefaultTrapClass != nullptr;
}

FText ADMBaseCharacter::GetAvailableTrapDisplayName() const
{
	if (AvailableTrapDisplayName.IsEmpty())
	{
		const FString ResolvedName = ResolveTrapDisplayName(DefaultTrapClass);
		return ResolvedName.IsEmpty() ? FText::FromString(TEXT("No Trap")) : FText::FromString(ResolvedName);
	}

	return FText::FromString(AvailableTrapDisplayName);
}

FString ADMBaseCharacter::ResolveTrapDisplayName(TSubclassOf<ADMBaseTrap> TrapClass) const
{
	if (TrapClass == nullptr)
	{
		return FString();
	}

	const ADMBaseTrap* TrapDefaults = TrapClass->GetDefaultObject<ADMBaseTrap>();
	if (TrapDefaults == nullptr)
	{
		return TrapClass->GetName();
	}

	if (!TrapDefaults->TrapDisplayName.IsEmpty())
	{
		return TrapDefaults->TrapDisplayName.ToString();
	}

	return TrapClass->GetName();
}

bool ADMBaseCharacter::FindTrapPlacementTransform(FTransform& OutTransform) const
{
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return false;
	}

	FVector ForwardDirection = GetActorForwardVector();
	if (Controller)
	{
		const FRotator ControlRotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
		ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	}

	ForwardDirection.Z = 0.f;
	ForwardDirection.Normalize();

	const FVector TargetLocation = GetActorLocation() + ForwardDirection * TrapPlacementDistance;
	const FVector TraceStart = TargetLocation + FVector(0.f, 0.f, TrapPlacementTraceHeight);
	const FVector TraceEnd = TargetLocation - FVector(0.f, 0.f, TrapPlacementTraceDepth);

	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(DMTrapPlacement), true);
	Params.AddIgnoredActor(this);

	if (!World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params))
	{
		return false;
	}

	if (!Hit.bBlockingHit)
	{
		return false;
	}

	const FRotator TrapRotation = FRotationMatrix::MakeFromZX(Hit.ImpactNormal, ForwardDirection).Rotator();
	OutTransform = FTransform(TrapRotation, Hit.ImpactPoint);
	return true;
}

void ADMBaseCharacter::RegisterPlacedTrap(ADMBaseTrap* Trap)
{
	if (Trap == nullptr)
	{
		return;
	}

	PlacedTraps.RemoveAll([](const TWeakObjectPtr<ADMBaseTrap>& ExistingTrap)
		{
			return !ExistingTrap.IsValid();
		});

	PlacedTraps.Add(Trap);

	while (MaxPlacedTraps > 0 && PlacedTraps.Num() > MaxPlacedTraps)
	{
		TWeakObjectPtr<ADMBaseTrap> OldestTrap = PlacedTraps[0];
		PlacedTraps.RemoveAt(0);

		if (OldestTrap.IsValid())
		{
			OldestTrap->Destroy();
		}
	}
}

bool ADMBaseCharacter::PlaceTrap()
{
	if (!HasAuthority() || !CanPlaceTrap())
	{
		return false;
	}

	FTransform TrapTransform;
	if (!FindTrapPlacementTransform(TrapTransform))
	{
		return false;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ADMBaseTrap* Trap = GetWorld()->SpawnActor<ADMBaseTrap>(DefaultTrapClass, TrapTransform, SpawnParams);
	if (Trap == nullptr)
	{
		return false;
	}

	LastTrapPlacementTime = GetWorld() ? GetWorld()->GetTimeSeconds() : LastTrapPlacementTime;
	Trap->SetPlacedBy(this);
	RegisterPlacedTrap(Trap);
	OnTrapPlaced(Trap);
	SetAvailableTrapClass(nullptr);
	return true;
}

void ADMBaseCharacter::HandleReloadStarted()
{
	bIsReloading = true;
	StopFire();

	if (bLockMovementDuringReload)
	{
		bIsRunning = false;
		ApplyRunningState(false);

		if (GetCharacterMovement())
		{
			GetCharacterMovement()->StopMovementImmediately();
		}
	}

	if (ReloadMontage && GetMesh())
	{
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			AnimInstance->Montage_Play(ReloadMontage);
		}
	}
}

void ADMBaseCharacter::HandleReloadFinished()
{
	bIsReloading = false;

	if (GetMesh())
	{
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			if (ReloadMontage && AnimInstance->Montage_IsPlaying(ReloadMontage))
			{
				AnimInstance->Montage_Stop(0.15f, ReloadMontage);
			}
		}
	}

	ApplyRunningState(bIsRunning);
}

bool ADMBaseCharacter::GetWeaponLeftHandIKTransform(FVector& OutLocation, FRotator& OutRotation) const
{
	OutLocation = FVector::ZeroVector;
	OutRotation = FRotator::ZeroRotator;

	if (CurrentWeapon == nullptr || CurrentWeapon->WeaponMesh == nullptr || GetMesh() == nullptr)
	{
		return false;
	}

	if (!CurrentWeapon->WeaponMesh->DoesSocketExist(CurrentWeapon->LeftHandIKSocketName))
	{
		return false;
	}

	const FVector WorldLocation = CurrentWeapon->WeaponMesh->GetSocketLocation(CurrentWeapon->LeftHandIKSocketName);
	const FRotator WorldRotation = CurrentWeapon->WeaponMesh->GetSocketRotation(CurrentWeapon->LeftHandIKSocketName);
	GetMesh()->TransformToBoneSpace(LeftHandIKTargetBone, WorldLocation, WorldRotation, OutLocation, OutRotation);
	return true;
}

void ADMBaseCharacter::ApplySpeedBoost(float Multiplier, float Duration)
{
	if (GetCharacterMovement() == nullptr)
	{
		return;
	}

	TemporaryMovementMultiplier = FMath::Max(0.1f, Multiplier);
	bSpeedBoostActive = true;
	bJumpBoostActive = true;

	GetCharacterMovement()->JumpZVelocity = CachedBaseJumpZVelocity * FMath::Max(0.1f, SpeedBoostJumpMultiplier);
	ApplyRunningState(bIsRunning);
	MulticastSpeedBoostStarted(TemporaryMovementMultiplier, SpeedBoostJumpMultiplier, Duration);

	GetWorld()->GetTimerManager().SetTimer(MovementEffectTimerHandle, this, &ADMBaseCharacter::EndTemporaryMovementEffect, Duration, false);
}

void ADMBaseCharacter::ApplySlow(float Multiplier, float Duration)
{
	if (GetCharacterMovement() == nullptr)
	{
		return;
	}

	TemporaryMovementMultiplier = FMath::Clamp(Multiplier, 0.1f, 1.0f);
	bSpeedBoostActive = false;
	bJumpBoostActive = false;

	GetCharacterMovement()->JumpZVelocity = CachedBaseJumpZVelocity;
	ApplyRunningState(bIsRunning);

	GetWorld()->GetTimerManager().SetTimer(MovementEffectTimerHandle, this, &ADMBaseCharacter::EndTemporaryMovementEffect, Duration, false);
}

void ADMBaseCharacter::ApplyDamageBoost(float Multiplier, float Duration)
{
	DamageMultiplier = Multiplier;
	MulticastDamageBoostStarted(Multiplier, Duration);
	GetWorld()->GetTimerManager().SetTimer(DamageBoostTimerHandle, this, &ADMBaseCharacter::EndDamageBoost, Duration, false);
}

void ADMBaseCharacter::ApplyBlindness(float Duration)
{
	if (Duration <= 0.f)
	{
		return;
	}

	ADMPlayerController* DMController = Cast<ADMPlayerController>(GetController());
	if (DMController)
	{
		DMController->ClientApplyBlindness(Duration);
	}
}

void ADMBaseCharacter::EndTemporaryMovementEffect()
{
	const bool bWasSpeedBoostActive = bSpeedBoostActive;

	TemporaryMovementMultiplier = 1.0f;
	bSpeedBoostActive = false;
	bJumpBoostActive = false;

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->JumpZVelocity = CachedBaseJumpZVelocity;
		ApplyRunningState(bIsRunning);
	}

	if (bWasSpeedBoostActive)
	{
		MulticastSpeedBoostEnded();
	}
}

void ADMBaseCharacter::EndDamageBoost()
{
	DamageMultiplier = 1.0f;
	MulticastDamageBoostEnded();
}

void ADMBaseCharacter::SetCharacterVisibilityForSkill(bool bVisible)
{
	bSkillHidden = !bVisible;
	ApplySkillVisibility();
}

void ADMBaseCharacter::ResetAfterSpawn()
{
	EndSlide();

	bIsDead = false;
	bIsAiming = false;
	bIsRunning = false;
	bIsReloading = false;
	bIsSliding = false;
	bSkillHidden = false;
	DamageMultiplier = 1.0f;
	Health = MaxHealth;
	SetActorEnableCollision(true);
	SetActorHiddenInGame(false);
	ApplyAimState(false);
	ApplyRunningState(false);
	ApplySkillVisibility();

	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	if (GetMesh())
	{
		GetMesh()->SetSimulatePhysics(false);
		GetMesh()->SetAllBodiesSimulatePhysics(false);
		GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));
	}

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		GetCharacterMovement()->JumpZVelocity = CachedBaseJumpZVelocity;
	}

	TemporaryMovementMultiplier = 1.0f;
	bSpeedBoostActive = false;
	bJumpBoostActive = false;
	ApplyRunningState(false);

	OnHealthChanged();
}

void ADMBaseCharacter::MulticastHandleDeath_Implementation()
{
	EndSlide();

	bIsDead = true;
	bIsAiming = false;
	bIsRunning = false;
	bIsReloading = false;
	bIsSliding = false;
	ApplyAimState(false);
	ApplyRunningState(false);

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->DisableMovement();
	}

	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (bRagdollOnDeath)
	{
		ApplyDeathRagdoll();
	}

	OnDeath();
}

void ADMBaseCharacter::MulticastSpeedBoostStarted_Implementation(float SpeedMultiplier, float JumpMultiplier, float Duration)
{
	OnSpeedBoostStarted(SpeedMultiplier, JumpMultiplier, Duration);
}

void ADMBaseCharacter::MulticastSpeedBoostEnded_Implementation()
{
	OnSpeedBoostEnded();
}

void ADMBaseCharacter::MulticastDamageBoostStarted_Implementation(float DamageMultiplierValue, float Duration)
{
	OnDamageBoostStarted(DamageMultiplierValue, Duration);
}

void ADMBaseCharacter::MulticastDamageBoostEnded_Implementation()
{
	OnDamageBoostEnded();
}

void ADMBaseCharacter::MulticastSlideStarted_Implementation(FVector_NetQuantizeNormal SlideDirection)
{
	BeginSlide(SlideDirection);
}

void ADMBaseCharacter::ApplyDeathRagdoll()
{
	if (GetMesh() == nullptr)
	{
		return;
	}

	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
}

void ADMBaseCharacter::OnRep_Health()
{
	OnHealthChanged();
}

void ADMBaseCharacter::OnRep_Aiming()
{
	ApplyAimState(bIsAiming);
}

void ADMBaseCharacter::OnRep_Running()
{
	ApplyRunningState(bIsRunning);
}

void ADMBaseCharacter::OnRep_SkillHidden()
{
	ApplySkillVisibility();
}

void ADMBaseCharacter::OnRep_AvailableTrapClass()
{
	OnTrapClassChanged(DefaultTrapClass);
}

void ADMBaseCharacter::ApplyAimState(bool bNewAiming)
{
	if (bUseCrosshairAimRotation)
	{
		bUseControllerRotationYaw = false;

		if (GetCharacterMovement())
		{
			GetCharacterMovement()->bOrientRotationToMovement = false;
			GetCharacterMovement()->bUseControllerDesiredRotation = false;
		}

		return;
	}

	bUseControllerRotationYaw = bNewAiming;

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->bOrientRotationToMovement = !bNewAiming;
		GetCharacterMovement()->bUseControllerDesiredRotation = bNewAiming;
	}
}

void ADMBaseCharacter::ApplyRunningState(bool bNewRunning)
{
	if (GetCharacterMovement())
	{
		const float BaseSpeed = bNewRunning ? RunSpeed : WalkSpeed;
		GetCharacterMovement()->MaxWalkSpeed = BaseSpeed * TemporaryMovementMultiplier;
	}
}

void ADMBaseCharacter::UpdateCameraSettings(float DeltaTime)
{
	if (SpringArm == nullptr || Camera == nullptr)
	{
		return;
	}

	const float TargetArmLength = bIsAiming ? AimCameraArmLength : NormalCameraArmLength;
	const FVector TargetSocketOffset = bIsAiming ? AimCameraSocketOffset : NormalCameraSocketOffset;
	const float TargetFOV = bIsAiming ? AimCameraFOV : NormalCameraFOV;
	const float InterpSpeed = FMath::Max(0.1f, CameraAimInterpSpeed);

	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, TargetArmLength, DeltaTime, InterpSpeed);
	SpringArm->SocketOffset = FMath::VInterpTo(SpringArm->SocketOffset, TargetSocketOffset, DeltaTime, InterpSpeed);
	Camera->SetFieldOfView(FMath::FInterpTo(Camera->FieldOfView, TargetFOV, DeltaTime, InterpSpeed));
}

void ADMBaseCharacter::ApplySkillVisibility()
{
	SetActorHiddenInGame(bSkillHidden);

	if (CurrentWeapon)
	{
		if (bSkillHidden)
		{
			CurrentWeapon->StopFire();
		}

		CurrentWeapon->SetActorHiddenInGame(bSkillHidden);
	}
}

void ADMBaseCharacter::SpawnDefaultWeapon()
{
	if (DefaultWeaponClass == nullptr || CurrentWeapon != nullptr || GetWorld() == nullptr)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;
	CurrentWeapon = GetWorld()->SpawnActor<ADMBaseWeapon>(DefaultWeaponClass, SpawnParams);

	if (CurrentWeapon)
	{
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("weaponSocket"));
		CurrentWeapon->SetOwningCharacter(this);
		CurrentWeapon->SetActorHiddenInGame(bSkillHidden);
		OnWeaponChanged();
	}
}
