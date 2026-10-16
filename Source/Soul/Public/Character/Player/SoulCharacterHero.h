// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Character/SoulCharacterBase.h"
#include "SoulType/SoulMovementTypes.h"
#include "SoulCharacterHero.generated.h"

class ASoulWeapon;
class USoulAbilitySystemComponentBase;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;
class USpringArmComponent;
class UCameraComponent;
class UGameplayAbility;
/**
 * 
 */
UCLASS()
class SOUL_API ASoulCharacterHero : public ASoulCharacterBase, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ASoulCharacterHero();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	USoulAbilitySystemComponentBase* GetSoulAbilitySystemComponent() const;

	ASoulWeapon* GetEquippedWeapon() const;

	bool SwitchToNextWeapon();

	void EquipWeaponAtIndex(int32 NewIndex);
	void UnequipWeapon();

	UEnhancedInputComponent*
	GetEnhancedInputComponent() const;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	ESoulMovementGait CurrentGait = ESoulMovementGait::Walking;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	FSoulMovementProfile DefaultMovementProfile;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetMovementGait(ESoulMovementGait NewGait);

	UFUNCTION(BlueprintPure, Category = "Movement")
	ESoulMovementGait GetMovementGait() const;

	const FSoulMovementProfile& GetCurrentMovementProfile() const;

	void ApplyCurrentMovementSettings();

	

protected:
	virtual void BeginPlay() override;
	virtual void PawnClientRestart() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	void InitializeAbilitySystem();

	void GrantDefaultAbilities();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TArray<TSubclassOf<ASoulWeapon>> WeaponClasses;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Weapon")
	TArray<TObjectPtr<ASoulWeapon>> WeaponSlots;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	int32 EquippedWeaponIndex = INDEX_NONE;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> SwitchWeaponAction;

	virtual void EndPlay(
		const EEndPlayReason::Type EndPlayReason) override;
	
	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<ASoulWeapon> EquippedWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Ability System|GAS")
	TSubclassOf<UGameplayAbility> SwitchWeaponAbility;

	// Component
	TWeakObjectPtr<UEnhancedInputComponent>
	EnhancedInputComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	//Input
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> MappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> JogAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;


private:
	void Move(const FInputActionValue& Value);
	void StartJog();
	void StopJog();
	void Look(const FInputActionValue& Value);

	void SpawnWeaponSlots();

	void HandleSwitchWeaponInput(const FInputActionValue& Value);
};

