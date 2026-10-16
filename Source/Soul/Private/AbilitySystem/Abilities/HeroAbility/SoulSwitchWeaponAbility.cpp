// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/HeroAbility/SoulSwitchWeaponAbility.h"

#include "Character/Player/SoulCharacterHero.h"
#include "GameInfo/SoulGameplayTags.h"


USoulSwitchWeaponAbility::USoulSwitchWeaponAbility()
{
	InstancingPolicy =
	EGameplayAbilityInstancingPolicy::InstancedPerExecution;

	NetExecutionPolicy =
		EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	AbilityTags.AddTag(
		SoulGameplayTags::Ability_Weapon_Switch);

	ActivationOwnedTags.AddTag(
		SoulGameplayTags::State_Weapon_Switching);

	ActivationBlockedTags.AddTag(
		SoulGameplayTags::State_Weapon_Switching);

	ActivationBlockedTags.AddTag(
	SoulGameplayTags::State_Weapon_StanceChanging);

	ActivationBlockedTags.AddTag(
	SoulGameplayTags::State_Weapon_GiantDrawStowing);
}


void USoulSwitchWeaponAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	ASoulCharacterHero* Hero =
		Cast<ASoulCharacterHero>(
			GetAvatarActorFromActorInfo());

	if (!Hero)
	{
		EndAbility(
			Handle,
			ActorInfo,
			ActivationInfo,
			true,
			true);

		return;
	}

	if (!CommitAbility(
		Handle,
		ActorInfo,
		ActivationInfo))
	{
		EndAbility(
			Handle,
			ActorInfo,
			ActivationInfo,
			true,
			true);

		return;
	}

	Hero->SwitchToNextWeapon();

	EndAbility(
		Handle,
		ActorInfo,
		ActivationInfo,
		true,
		false);
}
