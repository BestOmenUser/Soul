// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/SoulCharacterHero.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/SoulAbilitySystemComponentBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameInfo/EvilPlayerState.h"
#include "Weapon/SoulWeapon.h"
#include "Weapon/DerivedWeapon/GiantWeapon.h"

class UEnhancedInputLocalPlayerSubsystem;

ASoulCharacterHero::ASoulCharacterHero()
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 300.f, 0.f);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 350.f;
	CameraBoom->TargetOffset = FVector(0.f, 0.f, 70.f);
	CameraBoom->SocketOffset = FVector(0.f, 35.f, 0.f);
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bDoCollisionTest = true;  

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
}

UAbilitySystemComponent* ASoulCharacterHero::GetAbilitySystemComponent() const
{
	const AEvilPlayerState* SoulPlayerState =
	GetPlayerState<AEvilPlayerState>();

	return SoulPlayerState
		? SoulPlayerState->GetAbilitySystemComponent()
		: nullptr;
}

USoulAbilitySystemComponentBase* ASoulCharacterHero::GetSoulAbilitySystemComponent() const
{
	const AEvilPlayerState* SoulPlayerState =
	GetPlayerState<AEvilPlayerState>();

	return SoulPlayerState
		? SoulPlayerState->GetSoulAbilitySystemComponent()
		: nullptr;
}

ASoulWeapon* ASoulCharacterHero::GetEquippedWeapon() const
{
	return EquippedWeapon;
}

bool ASoulCharacterHero::SwitchToNextWeapon()
{
	const int32 WeaponCount =
WeaponSlots.Num();

	if (WeaponCount < 2)
	{
		return false;
	}

	const int32 StartIndex =
		EquippedWeaponIndex == INDEX_NONE
		? -1
		: EquippedWeaponIndex;

	for (int32 Offset = 1;
		Offset <= WeaponCount;
		++Offset)
	{
		int32 CandidateIndex =
			(StartIndex + Offset) % WeaponCount;

		if (CandidateIndex < 0)
		{
			CandidateIndex += WeaponCount;
		}

		if (IsValid(WeaponSlots[CandidateIndex]) && EquippedWeapon != WeaponSlots[CandidateIndex])
		{
			EquipWeaponAtIndex(CandidateIndex);
			return true;
		}
	}
	return  false;
}

void ASoulCharacterHero::EquipWeaponAtIndex(int32 NewIndex)
{
	if (!WeaponSlots.IsValidIndex(NewIndex))
	{
		return;
	}

	ASoulWeapon* NewWeapon =
		WeaponSlots[NewIndex];

	if (!IsValid(NewWeapon))
	{
		return;
	}

	if (EquippedWeapon == NewWeapon)
	{
		return;
	}

	if (EquippedWeapon)
	{
		UnequipWeapon();
	}

	EquippedWeapon = NewWeapon;
	EquippedWeaponIndex = NewIndex;

	EquippedWeapon->EquipToCharacter(this);
	ApplyCurrentMovementSettings();
}

void ASoulCharacterHero::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (EquippedWeapon)
	{
		UnequipWeapon();
	}

	for (ASoulWeapon* Weapon : WeaponSlots)
	{
		if (IsValid(Weapon))
		{
			Weapon->Destroy();
		}
	}

	WeaponSlots.Empty();

	Super::EndPlay(EndPlayReason);
}

void ASoulCharacterHero::SpawnWeaponSlots()
{
	if (!GetWorld() || WeaponSlots.Num() > 0)
	{
		return;
	}

	for (const TSubclassOf<ASoulWeapon>& WeaponClass : WeaponClasses)
	{
		if (!WeaponClass)
		{
			continue;
		}

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = this;
		SpawnParameters.Instigator = this;
		SpawnParameters.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ASoulWeapon* NewWeapon =
			GetWorld()->SpawnActor<ASoulWeapon>(
				WeaponClass,
				GetActorLocation(),
				GetActorRotation(),
				SpawnParameters);

		if (NewWeapon)
		{
			NewWeapon->SetActorHiddenInGame(true);
			WeaponSlots.Add(NewWeapon);
		}
	}
}

void ASoulCharacterHero::HandleSwitchWeaponInput(const FInputActionValue& Value)
{
	if (!SwitchWeaponAbility)
	{
		return;
	}

	if (USoulAbilitySystemComponentBase* ASC =
		GetSoulAbilitySystemComponent())
	{
		ASC->TryActivateAbilityByClass(
			SwitchWeaponAbility);
	}
}



void ASoulCharacterHero::UnequipWeapon()
{
	if (!EquippedWeapon)
	{
		return;
	}

	EquippedWeapon->UnequipFromCharacter();
	EquippedWeapon = nullptr;
	EquippedWeaponIndex = INDEX_NONE;
	ApplyCurrentMovementSettings();
}

UEnhancedInputComponent* ASoulCharacterHero::GetEnhancedInputComponent() const
{
	return EnhancedInputComponent.Get();
}

void ASoulCharacterHero::SetMovementGait(ESoulMovementGait NewGait)
{
	if (CurrentGait == NewGait)
	{
		return;
	}

	CurrentGait = NewGait;
	ApplyCurrentMovementSettings();
}

ESoulMovementGait ASoulCharacterHero::GetMovementGait() const
{
	return CurrentGait;
}

const FSoulMovementProfile& ASoulCharacterHero::GetCurrentMovementProfile() const
{
	if (const AGiantWeapon* Giant =
		Cast<AGiantWeapon>(EquippedWeapon))
	{
		return Giant->GetCurrentMovementProfile();
	}

	return DefaultMovementProfile;
}

void ASoulCharacterHero::ApplyCurrentMovementSettings()
{
	UCharacterMovementComponent* Movement =
		GetCharacterMovement();

	if (!Movement)
	{
		return;
	}

	const FSoulMovementProfile& Profile =
		GetCurrentMovementProfile();

	const FSoulGaitMovementSettings& Settings =
		Profile.GetSettings(CurrentGait);

	Movement->MaxWalkSpeed =
		Settings.MaxSpeed;

	Movement->MaxAcceleration =
		Settings.MaxAcceleration;

	Movement->BrakingDecelerationWalking =
		Settings.BrakingDeceleration;

	Movement->GroundFriction =
		Settings.GroundFriction;

	Movement->BrakingFrictionFactor =
		Settings.BrakingFrictionFactor;
}


void ASoulCharacterHero::BeginPlay()
{
	Super::BeginPlay();
	ApplyCurrentMovementSettings();
	
	SpawnWeaponSlots();

	if (HasAuthority() && WeaponSlots.Num() > 0 && !EquippedWeapon)
	{
		EquipWeaponAtIndex(0);
	}
}

void ASoulCharacterHero::PawnClientRestart()
{
	Super::PawnClientRestart();
	
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
		{
			if (auto* Subsystem =
				ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
			{
				if (MappingContext)
				{
					Subsystem->AddMappingContext(MappingContext, 0);
				}

				if (EquippedWeapon)
				{
					EquippedWeapon->RefreshLocalInput();
				}
			}
		}
	}
}

void ASoulCharacterHero::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!Input) return;

	EnhancedInputComponent = Input;
	

	if (!EquippedWeapon && WeaponSlots.Num() > 0)
	{
		EquipWeaponAtIndex(0);
	}

	if (EquippedWeapon)
	{
		EquippedWeapon->RefreshLocalInput();
	}

	if (SwitchWeaponAction)
	{
		Input->BindAction(
			SwitchWeaponAction,
			ETriggerEvent::Started,
			this,
			&ASoulCharacterHero::HandleSwitchWeaponInput);
	}
	
	
	if (MoveAction)
	{
		Input->BindAction(MoveAction, ETriggerEvent::Triggered,
			this, &ASoulCharacterHero::Move);
	}
	if (JogAction)
	{
		Input->BindAction(JogAction, ETriggerEvent::Started,
			this, &ASoulCharacterHero::StartJog);
		Input->BindAction(JogAction, ETriggerEvent::Completed,
			this, &ASoulCharacterHero::StopJog);
		Input->BindAction(JogAction, ETriggerEvent::Canceled,
			this, &ASoulCharacterHero::StopJog);
	}
	if (LookAction)
	{
		Input->BindAction(LookAction, ETriggerEvent::Triggered,
			this, &ASoulCharacterHero::Look);
	}

}

void ASoulCharacterHero::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitializeAbilitySystem();
	GrantDefaultAbilities();
}

void ASoulCharacterHero::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitializeAbilitySystem();
}

void ASoulCharacterHero::InitializeAbilitySystem()
{
	AEvilPlayerState* SoulPlayerState =
		GetPlayerState<AEvilPlayerState>();

	if (!SoulPlayerState)
	{
		return;
	}

	if (USoulAbilitySystemComponentBase* ASC =
		SoulPlayerState->GetSoulAbilitySystemComponent())
	{
		ASC->InitAbilityActorInfo(SoulPlayerState, this);
	}

	if (HasAuthority() && EquippedWeapon)
	{
		EquippedWeapon->GrantWeaponAbility();
	}
}

void ASoulCharacterHero::GrantDefaultAbilities()
{
	if (!HasAuthority() || !SwitchWeaponAbility)
	{
		return;
	}

	USoulAbilitySystemComponentBase* ASC =
		GetSoulAbilitySystemComponent();

	if (!ASC)
	{
		return;
	}

	if (ASC->FindAbilitySpecFromClass(
		SwitchWeaponAbility))
	{
		return;
	}

	ASC->GiveAbility(
		FGameplayAbilitySpec(
			SwitchWeaponAbility,
			1,
			INDEX_NONE,
			this));
}



void ASoulCharacterHero::Move(const FInputActionValue& Value)
{
	if (!Controller) return;

	const FVector2D Axis = Value.Get<FVector2D>();
	const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
	const FRotationMatrix RotationMatrix(YawRotation);

	AddMovementInput(RotationMatrix.GetUnitAxis(EAxis::X), Axis.Y);
	AddMovementInput(RotationMatrix.GetUnitAxis(EAxis::Y), Axis.X);
}

void ASoulCharacterHero::StartJog()
{
	SetMovementGait(ESoulMovementGait::Jogging);
}

void ASoulCharacterHero::StopJog()
{
	SetMovementGait(ESoulMovementGait::Walking);
}

void ASoulCharacterHero::Look(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	AddControllerYawInput(Axis.X);
	AddControllerPitchInput(Axis.Y);
}
