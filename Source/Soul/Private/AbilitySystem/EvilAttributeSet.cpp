// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/EvilAttributeSet.h"

#include "GameplayEffectExtension.h"

UEvilAttributeSet::UEvilAttributeSet()
{
	
}

void UEvilAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue =FMath::Clamp(NewValue,0.f,GetMaxHealth());
	}else if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue,0.f,GetMaxStamina());
	}else if (Attribute == GetIncomingDamageAttribute() && GetIncomingDamage()>0.f)
	{
		SetHealth(NewValue-GetIncomingDamage());
	}
}

void UEvilAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(
			FMath::Clamp(
				GetHealth(),
				0.0f,
				GetMaxHealth()
			)
		);
	}
	else if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(
			FMath::Clamp(
				GetStamina(),
				0.0f,
				GetMaxStamina()
			)
		);
	}
}
