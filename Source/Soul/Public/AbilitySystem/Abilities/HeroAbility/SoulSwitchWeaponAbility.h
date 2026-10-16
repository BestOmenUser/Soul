// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/SoulGameplayAbility.h"
#include "SoulSwitchWeaponAbility.generated.h"

/**
 * 
 */
UCLASS()
class SOUL_API USoulSwitchWeaponAbility
	: public USoulGameplayAbility
{
	GENERATED_BODY()

public:
	USoulSwitchWeaponAbility();

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
};