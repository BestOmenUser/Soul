// Fill out your copyright notice in the Description page of Project Settings.




#include "Anim/AnimNotify/GiantWeaponAttachNotify.h"

#include "AbilitySystemComponent.h"
#include "Character/Player/SoulCharacterHero.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameInfo/SoulGameplayTags.h"
#include "Weapon/DerivedWeapon/GiantWeapon.h"

void UGiantWeaponAttachNotify::Notify(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	ASoulCharacterHero* Hero = MeshComp
		? Cast<ASoulCharacterHero>(MeshComp->GetOwner())
		: nullptr;

	if (!Hero || Hero->GetMesh() != MeshComp)
	{
		return;
	}

	UAbilitySystemComponent* ASC = Hero->GetAbilitySystemComponent();
	if (!ASC || !ASC->HasMatchingGameplayTag(
			SoulGameplayTags::State_Weapon_GiantDrawStowing))
	{
		return;
	}

	AGiantWeapon* Giant =
		Cast<AGiantWeapon>(Hero->GetEquippedWeapon());

	if (!Giant)
	{
		return;
	}

	bool bAttached = false;

	if (Target == EGiantWeaponAttachTarget::Hand && Giant->IsStowed())
	{
		bAttached = Giant->DrawFromBack();
	}
	else if (Target == EGiantWeaponAttachTarget::Back && !Giant->IsStowed())
	{
		bAttached = Giant->StowOnBack(Hero);
	}

	if (!bAttached)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("Giant weapon attach notify failed: %s"),
			*GetNameSafe(Giant));
	}
}