// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GiantWeaponAttachNotify.generated.h"

UENUM(BlueprintType)
enum class EGiantWeaponAttachTarget : uint8
{
	Hand,
	Back
};

UCLASS(meta = (DisplayName = "Giant Weapon Attach"))
class SOUL_API UGiantWeaponAttachNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	EGiantWeaponAttachTarget Target = EGiantWeaponAttachTarget::Hand;
};