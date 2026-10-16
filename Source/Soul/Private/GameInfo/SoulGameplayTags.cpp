// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInfo/SoulGameplayTags.h"

namespace SoulGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(
		Ability_Weapon_Switch,
		"Ability.Weapon.Switch",
		"切换武器能力");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(
		State_Weapon_Switching,
		"State.Weapon.Switching",
		"正在切换武器");


	UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Ability_Weapon_GiantStance,
	"Ability.Weapon.GiantStance",
	"切换武器姿态能力");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(
		State_Weapon_StanceChanging,
		"State.Weapon.StanceChanging",
		"正在切换武器姿态");

	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Ability_Weapon_GiantDrawStow,
	"Ability.Weapon.GiantDrawStow",
	"巨剑拔剑和收剑能力");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(
		State_Weapon_GiantDrawStowing,
		"State.Weapon.GiantDrawStowing",
		"巨剑正在拔剑或收剑");
}