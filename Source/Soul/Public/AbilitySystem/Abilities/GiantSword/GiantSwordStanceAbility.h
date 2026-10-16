// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/SoulGameplayAbility.h"
#include "GiantSwordStanceAbility.generated.h"

class AGiantWeapon;
/**
 * 
 */
UCLASS()
class SOUL_API UGiantSwordStanceAbility : public USoulGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGiantSwordStanceAbility();

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageCancelled();

	UFUNCTION()
	void OnMontageBlendOut();
	

private:
	void FinishStance(bool bCancelled);

	TWeakObjectPtr<AGiantWeapon> PendingWeapon;
	bool bFinishing = false;

	bool bStanceCommitted = false;
};
