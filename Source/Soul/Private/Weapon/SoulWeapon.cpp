#include "Weapon/SoulWeapon.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Character/Player/SoulCharacterHero.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"

ASoulWeapon::ASoulWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASoulWeapon::ApplyCurrentAnimLayer()
{
	ASoulCharacterHero* Hero = EquippedHero.Get();
	if (!Hero || !DefaultAnimLayerClass || !Hero->GetMesh())
	{
		return;
	}

	if (UAnimInstance* Anim = Hero->GetMesh()->GetAnimInstance())
	{
		Anim->LinkAnimClassLayers(DefaultAnimLayerClass);
	}
}

void ASoulWeapon::GrantWeaponAbility()
{
	ASoulCharacterHero* Hero = EquippedHero.Get();
	if (!Hero || !Hero->HasAuthority())
	{
		return;
	}

	UAbilitySystemComponent* ASC =
		Hero->GetAbilitySystemComponent();
	if (!ASC)
	{
		return;
	}

	if (SwitchStanceAbility &&
		!SwitchStanceAbilityHandle.IsValid())
	{
		SwitchStanceAbilityHandle = ASC->GiveAbility(
			FGameplayAbilitySpec(SwitchStanceAbility, 1));
	}

	if (DrawStowAbility &&
		!DrawStowAbilityHandle.IsValid())
	{
		DrawStowAbilityHandle = ASC->GiveAbility(
			FGameplayAbilitySpec(DrawStowAbility, 1));
	}
}

void ASoulWeapon::RemoveWeaponAbility()
{
	ASoulCharacterHero* Hero = EquippedHero.Get();

	if (Hero && Hero->HasAuthority() &&
		SwitchStanceAbilityHandle.IsValid())
	{
		if (UAbilitySystemComponent* ASC =
			Hero->GetAbilitySystemComponent())
		{
			ASC->CancelAbilityHandle(SwitchStanceAbilityHandle);
			ASC->ClearAbility(SwitchStanceAbilityHandle);
		}
	}

	if (Hero && Hero->HasAuthority() &&
	DrawStowAbilityHandle.IsValid())
	{
		if (UAbilitySystemComponent* ASC =
			Hero->GetAbilitySystemComponent())
		{
			ASC->CancelAbilityHandle(DrawStowAbilityHandle);
			ASC->ClearAbility(DrawStowAbilityHandle);
		}
	}

	SwitchStanceAbilityHandle = FGameplayAbilitySpecHandle();
	DrawStowAbilityHandle =	FGameplayAbilitySpecHandle();
}

const FSoulMovementProfile& ASoulWeapon::GetCurrentMovementProfile() const
{
	return DefaultMovementProfile;
}


void ASoulWeapon::EquipToCharacter(
	ASoulCharacterHero* NewOwner)
{
	if (!NewOwner)
	{
		return;
	}

	if (EquippedHero.IsValid() &&
		EquippedHero.Get() != NewOwner)
	{
		UnequipFromCharacter();
	}

	EquippedHero = NewOwner;
	GrantWeaponAbility();
	SetOwner(NewOwner);

	RefreshLocalInput();

	if (USkeletalMeshComponent* CharacterMesh =
	NewOwner->GetMesh())
	{
		if (!EquipSocketName.IsNone() &&
			CharacterMesh->DoesSocketExist(EquipSocketName))
		{
			AttachToComponent(CharacterMesh,FAttachmentTransformRules::SnapToTargetNotIncludingScale,EquipSocketName);
		}
	}
	
	SetActorHiddenInGame(false);
	ApplyCurrentAnimLayer();
}

void ASoulWeapon::UnequipFromCharacter()
{
	RemoveWeaponAbility();
	RemoveWeaponMappingContext();
	UnbindInput();

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	
	SetActorHiddenInGame(true);
	
	EquippedHero.Reset();
	SetOwner(nullptr);
}


void ASoulWeapon::AddWeaponMappingContext()
{
	ASoulCharacterHero* Hero =
		EquippedHero.Get();

	if (!Hero || !WeaponMappingContext)
	{
		return;
	}

	APlayerController* PlayerController =
		Cast<APlayerController>(
			Hero->GetController());

	if (!PlayerController)
	{
		return;
	}

	ULocalPlayer* LocalPlayer =
		PlayerController->GetLocalPlayer();

	if (!LocalPlayer)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(
				LocalPlayer);

	if (Subsystem &&
	!Subsystem->HasMappingContext(WeaponMappingContext))
	{
		Subsystem->AddMappingContext(
			WeaponMappingContext,
			MappingPriority);
	}
}


void ASoulWeapon::RemoveWeaponMappingContext()
{
	ASoulCharacterHero* Hero =
		EquippedHero.Get();

	if (!Hero || !WeaponMappingContext)
	{
		return;
	}

	APlayerController* PlayerController =
		Cast<APlayerController>(
			Hero->GetController());

	if (!PlayerController)
	{
		return;
	}

	ULocalPlayer* LocalPlayer =
		PlayerController->GetLocalPlayer();

	if (!LocalPlayer)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(
				LocalPlayer);

	if (Subsystem)
	{
		Subsystem->RemoveMappingContext(
			WeaponMappingContext);
	}
}


void ASoulWeapon::BindInputTo(
	UEnhancedInputComponent* InInputComponent)
{
	if (!InInputComponent)
	{
		return;
	}

	UnbindInput();

	if (StanceAction)
	{
		FEnhancedInputActionEventBinding& Binding =
			InInputComponent->BindAction(
				StanceAction,
				ETriggerEvent::Started,
				this,
				&ASoulWeapon::HandleStanceInput);

		StanceInputBindingHandle =
			Binding.GetHandle();
	}

	if (DrawStowAction)
	{
		FEnhancedInputActionEventBinding& Binding =
			InInputComponent->BindAction(
				DrawStowAction,
				ETriggerEvent::Started,
				this,
				&ASoulWeapon::HandleDrawStowInput);

		DrawStowInputBindingHandle =
			Binding.GetHandle();
	}

	BoundInputComponent = InInputComponent;
}


void ASoulWeapon::RefreshLocalInput()
{
	ASoulCharacterHero* Hero = EquippedHero.Get();
	if (!Hero)
	{
		return;
	}

	AddWeaponMappingContext();
	BindInputTo(Hero->GetEnhancedInputComponent());
}


void ASoulWeapon::UnbindInput()
{
	if (UEnhancedInputComponent* BoundInput =
		BoundInputComponent.Get())
	{
		if (StanceInputBindingHandle != INDEX_NONE)
		{
			BoundInput->RemoveBindingByHandle(
				StanceInputBindingHandle);
		}

		if (DrawStowInputBindingHandle != INDEX_NONE)
		{
			BoundInput->RemoveBindingByHandle(
				DrawStowInputBindingHandle);
		}
	}

	BoundInputComponent.Reset();
	StanceInputBindingHandle = INDEX_NONE;
	DrawStowInputBindingHandle = INDEX_NONE;
}


void ASoulWeapon::HandleStanceInput(
	const FInputActionValue& Value)
{
	if (!SwitchStanceAbility)
	{
		return;
	}

	if (ASoulCharacterHero* Hero = EquippedHero.Get())
	{
		if (UAbilitySystemComponent* ASC =
			Hero->GetAbilitySystemComponent())
		{
			ASC->TryActivateAbilityByClass(SwitchStanceAbility);
		}
	}
}

void ASoulWeapon::HandleDrawStowInput(const FInputActionValue& Value)
{
	if (!DrawStowAbility)
	{
		return;
	}

	if (ASoulCharacterHero* Hero = EquippedHero.Get())
	{
		if (UAbilitySystemComponent* ASC =
			Hero->GetAbilitySystemComponent())
		{
			ASC->TryActivateAbilityByClass(
				DrawStowAbility);
		}
	}
}



