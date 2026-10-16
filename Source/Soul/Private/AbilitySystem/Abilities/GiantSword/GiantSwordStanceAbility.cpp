// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GiantSword/GiantSwordStanceAbility.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/Player/SoulCharacterHero.h"
#include "GameInfo/SoulGameplayTags.h"
#include "Weapon/DerivedWeapon/GiantWeapon.h"

UGiantSwordStanceAbility::UGiantSwordStanceAbility()
{
	InstancingPolicy =
		EGameplayAbilityInstancingPolicy::InstancedPerExecution;
	NetExecutionPolicy =
		EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	AbilityTags.AddTag(
		SoulGameplayTags::Ability_Weapon_GiantStance);
	ActivationOwnedTags.AddTag(
		SoulGameplayTags::State_Weapon_StanceChanging);
	ActivationBlockedTags.AddTag(
		SoulGameplayTags::State_Weapon_StanceChanging);
	ActivationBlockedTags.AddTag(
		SoulGameplayTags::State_Weapon_Switching);
	ActivationBlockedTags.AddTag(
	SoulGameplayTags::State_Weapon_GiantDrawStowing);
}

void UGiantSwordStanceAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	ASoulCharacterHero* Hero =
		Cast<ASoulCharacterHero>(GetAvatarActorFromActorInfo());
	AGiantWeapon* Giant = Hero
		? Cast<AGiantWeapon>(Hero->GetEquippedWeapon())
		: nullptr;
	UAnimMontage* Montage = Giant
		? Giant->GetNextStanceMontage()
		: nullptr;

	if (!Montage ||
		!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	PendingWeapon = Giant;

	UAbilityTask_PlayMontageAndWait* Task =
		UAbilityTask_PlayMontageAndWait::
			CreatePlayMontageAndWaitProxy(
				this, NAME_None, Montage);

	if (!Task)
	{
		FinishStance(true);
		return;
	}

	Task->OnCompleted.AddDynamic(
		this, &UGiantSwordStanceAbility::OnMontageCompleted);
	Task->OnInterrupted.AddDynamic(
		this, &UGiantSwordStanceAbility::OnMontageCancelled);
	Task->OnCancelled.AddDynamic(
		this, &UGiantSwordStanceAbility::OnMontageCancelled);
	Task->OnBlendOut.AddDynamic(
	this, &UGiantSwordStanceAbility::OnMontageBlendOut);
	Task->ReadyForActivation();
}

void UGiantSwordStanceAbility::OnMontageCompleted()
{
	FinishStance(false);
}

void UGiantSwordStanceAbility::OnMontageCancelled()
{
	FinishStance(true);
}

void UGiantSwordStanceAbility::OnMontageBlendOut()
{
	if (!bFinishing && IsActive() &&
	!bStanceCommitted && PendingWeapon.IsValid())
	{
		bStanceCommitted = PendingWeapon->CommitNextStance();
	}
}

void UGiantSwordStanceAbility::FinishStance(bool bCancelled)
{
	if (bFinishing || !IsActive())
	{
		return;
	}

	bFinishing = true;

	const bool bSucceeded = !bCancelled && PendingWeapon.IsValid() &&bStanceCommitted;

	PendingWeapon.Reset();

	EndAbility(
		GetCurrentAbilitySpecHandle(),
		GetCurrentActorInfo(),
		GetCurrentActivationInfo(),
		true,
		!bSucceeded);
}
