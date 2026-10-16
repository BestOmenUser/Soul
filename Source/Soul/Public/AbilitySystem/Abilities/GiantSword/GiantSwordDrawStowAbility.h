// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/SoulGameplayAbility.h"
#include "GiantSwordDrawStowAbility.generated.h"

class AGiantWeapon;
class ASoulCharacterHero;

UCLASS()
class SOUL_API UGiantSwordDrawStowAbility
	: public USoulGameplayAbility
{
	GENERATED_BODY()

public:
	UGiantSwordDrawStowAbility();

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

private:
	void FinishDrawStow(bool bCancelled);

	TWeakObjectPtr<AGiantWeapon> PendingWeapon;
	TWeakObjectPtr<ASoulCharacterHero> PendingHero;

	bool bDrawing = false;
	bool bFinishing = false;
};