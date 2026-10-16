// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GiantSword/GiantSwordDrawStowAbility.h"

#include "AbilitySystem/Abilities/GiantSword/GiantSwordDrawStowAbility.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/Player/SoulCharacterHero.h"
#include "GameInfo/SoulGameplayTags.h"
#include "Weapon/DerivedWeapon/GiantWeapon.h"

UGiantSwordDrawStowAbility::UGiantSwordDrawStowAbility()
{
    InstancingPolicy =
        EGameplayAbilityInstancingPolicy::InstancedPerExecution;

    NetExecutionPolicy =
        EGameplayAbilityNetExecutionPolicy::LocalPredicted;

    AbilityTags.AddTag(
        SoulGameplayTags::Ability_Weapon_GiantDrawStow);

    ActivationOwnedTags.AddTag(
        SoulGameplayTags::State_Weapon_GiantDrawStowing);

    ActivationBlockedTags.AddTag(
        SoulGameplayTags::State_Weapon_GiantDrawStowing);

    ActivationBlockedTags.AddTag(
        SoulGameplayTags::State_Weapon_Switching);

    ActivationBlockedTags.AddTag(
        SoulGameplayTags::State_Weapon_StanceChanging);
}

void UGiantSwordDrawStowAbility::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    ASoulCharacterHero* Hero =
        Cast<ASoulCharacterHero>(
            GetAvatarActorFromActorInfo());

    AGiantWeapon* Giant = Hero
        ? Cast<AGiantWeapon>(
            Hero->GetEquippedWeapon())
        : nullptr;

    if (!Hero || !Giant)
    {
        EndAbility(
            Handle,
            ActorInfo,
            ActivationInfo,
            true,
            true);

        return;
    }

    bDrawing = Giant->IsStowed();

    UAnimMontage* Montage = bDrawing
        ? Giant->GetDrawMontage()
        : Giant->GetStowMontage();

    if (!Montage ||
        !CommitAbility(
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

    PendingHero = Hero;
    PendingWeapon = Giant;

    UAbilityTask_PlayMontageAndWait* Task =
        UAbilityTask_PlayMontageAndWait::
        CreatePlayMontageAndWaitProxy(
            this,
            NAME_None,
            Montage);

    if (!Task)
    {
        FinishDrawStow(true);
        return;
    }

    Task->OnCompleted.AddDynamic(
        this,
        &UGiantSwordDrawStowAbility::OnMontageCompleted);

    Task->OnInterrupted.AddDynamic(
        this,
        &UGiantSwordDrawStowAbility::OnMontageCancelled);

    Task->OnCancelled.AddDynamic(
        this,
        &UGiantSwordDrawStowAbility::OnMontageCancelled);

    Task->ReadyForActivation();
}


void UGiantSwordDrawStowAbility::OnMontageCompleted()
{
    FinishDrawStow(false);
}

void UGiantSwordDrawStowAbility::OnMontageCancelled()
{
    FinishDrawStow(true);
}

void UGiantSwordDrawStowAbility::FinishDrawStow(
    bool bCancelled)
{
    if (bFinishing || !IsActive())
    {
        return;
    }

    bFinishing = true;

    const bool bSucceeded = !bCancelled && PendingWeapon.IsValid() && PendingHero.IsValid() &&
                                PendingHero->GetEquippedWeapon() == PendingWeapon.Get() && (bDrawing ? !PendingWeapon->IsStowed() : PendingWeapon->IsStowed());

    PendingWeapon.Reset();
    PendingHero.Reset();

    EndAbility(
        GetCurrentAbilitySpecHandle(),
        GetCurrentActorInfo(),
        GetCurrentActivationInfo(),
        true,
        !bSucceeded);
}