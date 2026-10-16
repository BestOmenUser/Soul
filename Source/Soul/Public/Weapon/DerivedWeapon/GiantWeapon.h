// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoulType/SoulMovementTypes.h"
#include "Weapon/SoulWeapon.h"
#include "GiantWeapon.generated.h"



USTRUCT(BlueprintType)
struct FSoulStanceStep
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Stance")
	TSubclassOf<UAnimInstance> AnimLayerClass;

	// 从本数组项切换到下一项时播放
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Stance")
	TObjectPtr<UAnimMontage> TransitionMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Stance")
	TObjectPtr<UAnimMontage> StowMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Stance")
	TObjectPtr<UAnimMontage> DrawMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	FSoulMovementProfile MovementProfile;
};


UENUM(BlueprintType)
enum class EGiantWeaponPose : uint8
{
	OneHandA UMETA(DisplayName="单手 A"),
	OneHandB UMETA(DisplayName="单手 B"),
	TwoHandA UMETA(DisplayName="双手 A"),
	TwoHandB UMETA(DisplayName="双手 B"),
	OnBack   UMETA(DisplayName="背后收纳")
};



/**
 * 
 */
UCLASS()
class SOUL_API AGiantWeapon : public ASoulWeapon
{
	GENERATED_BODY()

public:
	virtual void ApplyCurrentAnimLayer() override;

	virtual void EquipToCharacter(ASoulCharacterHero* NewOwner) override;
	virtual void UnequipFromCharacter() override;
	bool StowOnBack(ASoulCharacterHero* Hero);
	bool DrawFromBack();

	virtual const FSoulMovementProfile& GetCurrentMovementProfile() const  override;

	UFUNCTION(BlueprintCallable, Category="Giant|Stance")
	UAnimMontage* GetNextStanceMontage() const;

	UFUNCTION(BlueprintCallable, Category="Giant|Stance")
	bool CommitNextStance();

	UFUNCTION(BlueprintPure, Category="Giant|DrawStow")
	bool IsStowed() const
	{
		return CurrentPose ==
			EGiantWeaponPose::OnBack;
	}

	UFUNCTION(BlueprintCallable, Category="Giant|DrawStow")
	UAnimMontage* GetStowMontage() const;

	UFUNCTION(BlueprintCallable, Category="Giant|DrawStow")
	UAnimMontage* GetDrawMontage() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Giant|Stance")
	TArray<FSoulStanceStep> Stances;

	UPROPERTY(EditDefaultsOnly, Category="Giant|Attachment")
	FName BackSocketName = NAME_None;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Giant|Stance")
	EGiantWeaponPose CurrentPose = EGiantWeaponPose::OneHandA;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Giant|Stance")
	EGiantWeaponPose LastHeldPose = EGiantWeaponPose::OneHandA;

};
