// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInfo/EvilPlayerState.h"
#include "AbilitySystem/SoulAbilitySystemComponentBase.h"
#include "AbilitySystem/EvilAttributeSet.h"


AEvilPlayerState::AEvilPlayerState()
{
	NetUpdateFrequency = 100.f;

	AbilitySystemComponent =
		CreateDefaultSubobject<USoulAbilitySystemComponentBase>(
			TEXT("AbilitySystemComponent"));

	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(
		EGameplayEffectReplicationMode::Mixed);

	AttributeSet =
		CreateDefaultSubobject<UEvilAttributeSet>(
			TEXT("AttributeSet"));
}

UAbilitySystemComponent* AEvilPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

USoulAbilitySystemComponentBase*
AEvilPlayerState::GetSoulAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}