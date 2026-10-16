// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoulMovementTypes.generated.h"


UENUM(BlueprintType)
enum class ESoulMovementGait : uint8
{
	Walking UMETA(DisplayName = "Walking"),
	Jogging UMETA(DisplayName = "Jogging")
};


USTRUCT(BlueprintType)
struct FSoulGaitMovementSettings
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float MaxSpeed = 230.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float MaxAcceleration = 2048.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float BrakingDeceleration = 1024.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float GroundFriction = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float BrakingFrictionFactor = 2.f;
};

USTRUCT(BlueprintType)
struct FSoulMovementProfile
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement|Walking")
	FSoulGaitMovementSettings Walking;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement|Jogging")
	FSoulGaitMovementSettings Jogging;

	const FSoulGaitMovementSettings& GetSettings(
		ESoulMovementGait Gait) const
	{
		return Gait == ESoulMovementGait::Jogging
			? Jogging
			: Walking;
	}
};