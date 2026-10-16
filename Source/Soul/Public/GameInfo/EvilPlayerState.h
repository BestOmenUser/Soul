// Fill out your copyright notice in the Description page of Project Settings.


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "EvilPlayerState.generated.h"

class UAbilitySystemComponent;
class USoulAbilitySystemComponentBase;
class UEvilAttributeSet;

UCLASS()
class SOUL_API AEvilPlayerState
	: public APlayerState
	, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AEvilPlayerState();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	USoulAbilitySystemComponentBase* GetSoulAbilitySystemComponent() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ability System")
	TObjectPtr<USoulAbilitySystemComponentBase> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UEvilAttributeSet> AttributeSet;
};