// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "EvilAttributeSet.generated.h"



#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


UCLASS()
class SOUL_API UEvilAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UEvilAttributeSet();
	// Basic Attribute
	UPROPERTY(BlueprintReadOnly,Category="Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UEvilAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly,Category="Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UEvilAttributeSet, MaxHealth)

	UPROPERTY(BlueprintReadOnly,Category="Attributes")
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UEvilAttributeSet, Stamina)

	UPROPERTY(BlueprintReadOnly,Category="Attributes")
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UEvilAttributeSet, MaxStamina)

	// Combat Attribute
	UPROPERTY(BlueprintReadOnly,Category="Attributes")
	FGameplayAttributeData Attack;
	ATTRIBUTE_ACCESSORS(UEvilAttributeSet,Attack)

	UPROPERTY(BlueprintReadOnly,Category="Attributes")
	FGameplayAttributeData Defense;
	ATTRIBUTE_ACCESSORS(UEvilAttributeSet,Defense)

	UPROPERTY(BlueprintReadOnly,Category="Attributes")
	FGameplayAttributeData CritRate;
	ATTRIBUTE_ACCESSORS(UEvilAttributeSet, CritRate)

	UPROPERTY(BlueprintReadOnly,Category="Attributes")
	FGameplayAttributeData CritDamage;
	ATTRIBUTE_ACCESSORS(UEvilAttributeSet, CritDamage)

	UPROPERTY(BlueprintReadOnly,Category="Attributes")
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(UEvilAttributeSet, IncomingDamage)
	
protected:
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute,float& NewValue) override;

	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData &Data) override;
};
