// Fill out your copyright notice in the Description page of Project Settings.
#include "Weapon/DerivedWeapon/GiantWeapon.h"
#include "Animation/AnimInstance.h"
#include "Character/Player/SoulCharacterHero.h"
#include "Components/SkeletalMeshComponent.h"

UAnimMontage* AGiantWeapon::GetNextStanceMontage() const
{
	const ASoulCharacterHero* Hero = GetEquippedHero();

	if (!Hero ||
		Hero->GetEquippedWeapon() != this ||
		CurrentPose == EGiantWeaponPose::OnBack)
	{
		return nullptr;
	}

	const int32 HeldPoseCount =
		static_cast<int32>(EGiantWeaponPose::OnBack);

	const int32 AvailableCount =
		FMath::Min(Stances.Num(), HeldPoseCount);

	if (AvailableCount < 2)
	{
		return nullptr;
	}

	const int32 CurrentIndex =
		static_cast<int32>(CurrentPose);

	if (CurrentIndex < 0 ||
		CurrentIndex >= AvailableCount)
	{
		return nullptr;
	}

	const int32 NextIndex =
		(CurrentIndex + 1) % AvailableCount;

	if (!Stances[NextIndex].AnimLayerClass ||
		!Stances[CurrentIndex].TransitionMontage)
	{
		return nullptr;
	}

	return Stances[CurrentIndex].TransitionMontage;
}


bool AGiantWeapon::CommitNextStance()
{
	ASoulCharacterHero* Hero = GetEquippedHero();

	if (!Hero ||
		Hero->GetEquippedWeapon() != this ||
		CurrentPose == EGiantWeaponPose::OnBack)
	{
		return false;
	}

	// OnBack 的枚举值为 4，代表前面有四种手持姿态
	const int32 HeldPoseCount =
		static_cast<int32>(EGiantWeaponPose::OnBack);

	// 允许暂时只配置部分姿态
	const int32 AvailableCount =
		FMath::Min(Stances.Num(), HeldPoseCount);

	// 至少需要两种姿态才能进行切换
	if (AvailableCount < 2)
	{
		return false;
	}

	int32 CurrentIndex =
		static_cast<int32>(CurrentPose);

	// 防止当前枚举超出目前已经配置的数组范围
	if (CurrentIndex < 0 ||
		CurrentIndex >= AvailableCount)
	{
		return false;
	}

	const int32 NextIndex =
		(CurrentIndex + 1) % AvailableCount;

	if (!Stances.IsValidIndex(NextIndex) ||
		!Stances[NextIndex].AnimLayerClass)
	{
		return false;
	}

	UAnimInstance* Anim = Hero->GetMesh()
		? Hero->GetMesh()->GetAnimInstance()
		: nullptr;

	if (!Anim)
	{
		return false;
	}

	Anim->LinkAnimClassLayers(
		Stances[NextIndex].AnimLayerClass);

	CurrentPose =
		static_cast<EGiantWeaponPose>(NextIndex);

	LastHeldPose = CurrentPose;
	
	Hero->ApplyCurrentMovementSettings();
	
	return true;
}

UAnimMontage* AGiantWeapon::GetStowMontage() const
{
	if (CurrentPose == EGiantWeaponPose::OnBack)
	{
		return nullptr;
	}

	const int32 Index = static_cast<int32>(CurrentPose);
	return Stances.IsValidIndex(Index)
		? Stances[Index].StowMontage
		: nullptr;
}

UAnimMontage* AGiantWeapon::GetDrawMontage() const
{
	if (CurrentPose != EGiantWeaponPose::OnBack)
	{
		return nullptr;
	}

	const int32 Index = static_cast<int32>(LastHeldPose);
	return Stances.IsValidIndex(Index)
		? Stances[Index].DrawMontage
		: nullptr;
}

void AGiantWeapon::ApplyCurrentAnimLayer()
{
	ASoulCharacterHero* Hero = GetEquippedHero();
	UAnimInstance* Anim = Hero && Hero->GetMesh()
		? Hero->GetMesh()->GetAnimInstance()
		: nullptr;

	const int32 Index = static_cast<int32>(CurrentPose);
	if (Anim &&
		Stances.IsValidIndex(Index) &&
		Stances[Index].AnimLayerClass)
	{
		Anim->LinkAnimClassLayers(Stances[Index].AnimLayerClass);
	}
}

void AGiantWeapon::EquipToCharacter(ASoulCharacterHero* NewOwner)
{
	if (CurrentPose == EGiantWeaponPose::OnBack)
	{
		CurrentPose = LastHeldPose;
	}

	// 基类负责手部附着、武器输入和动画层。
	Super::EquipToCharacter(NewOwner);
}

void AGiantWeapon::UnequipFromCharacter()
{
	ASoulCharacterHero* Hero = GetEquippedHero();

	// 基类负责清理输入和 GA；随后重新显示并挂到背后。
	Super::UnequipFromCharacter();

	if (Hero && !StowOnBack(Hero))
	{
		UE_LOG(LogTemp, Warning,
			TEXT("Failed to stow giant weapon on back."));
	}
}

bool AGiantWeapon::StowOnBack(ASoulCharacterHero* Hero)
{
	USkeletalMeshComponent* Mesh = Hero ? Hero->GetMesh() : nullptr;
	if (!Mesh || BackSocketName.IsNone() ||
		!Mesh->DoesSocketExist(BackSocketName))
	{
		return false;
	}

	if (CurrentPose != EGiantWeaponPose::OnBack)
	{
		LastHeldPose = CurrentPose;
	}

	AttachToComponent(
		Mesh,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		BackSocketName);

	CurrentPose = EGiantWeaponPose::OnBack;
	SetOwner(Hero);
	SetActorHiddenInGame(false);

	
	if (GetEquippedHero() == Hero &&
	Hero->GetEquippedWeapon() == this)
	{
		ApplyCurrentAnimLayer();
		Hero->ApplyCurrentMovementSettings();
	}
	return true;
}

bool AGiantWeapon::DrawFromBack()
{
	ASoulCharacterHero* Hero = GetEquippedHero();
	USkeletalMeshComponent* Mesh = Hero ? Hero->GetMesh() : nullptr;

	if (!Hero || Hero->GetEquippedWeapon() != this ||
		CurrentPose != EGiantWeaponPose::OnBack ||
		!Mesh || EquipSocketName.IsNone() ||
		!Mesh->DoesSocketExist(EquipSocketName))
	{
		return false;
	}

	AttachToComponent(
		Mesh,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		EquipSocketName);

	CurrentPose = LastHeldPose;
	ApplyCurrentAnimLayer();
	Hero->ApplyCurrentMovementSettings();
	return true;
}

const FSoulMovementProfile& AGiantWeapon::GetCurrentMovementProfile() const
{
	const int32 Index = static_cast<int32>(CurrentPose);

	return Stances.IsValidIndex(Index)
		? Stances[Index].MovementProfile
		: DefaultMovementProfile;
}

