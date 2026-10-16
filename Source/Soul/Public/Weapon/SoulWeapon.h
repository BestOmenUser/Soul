#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameFramework/Actor.h"
#include "SoulType/SoulMovementTypes.h"
#include "SoulWeapon.generated.h"

struct FSoulMovementProfile;
class UGameplayAbility;
class ASoulCharacterHero;
class UAnimInstance;
class UAnimMontage;
class UInputAction;
class UInputMappingContext;
class UEnhancedInputComponent;
struct FInputActionValue;


UCLASS()
class SOUL_API ASoulWeapon : public AActor
{
	GENERATED_BODY()

public:
	ASoulWeapon();

	virtual void ApplyCurrentAnimLayer();
	
	ASoulCharacterHero* GetEquippedHero() const
	{
		return EquippedHero.Get();
	}


	
	// 装备武器并启用武器输入
	virtual  void EquipToCharacter(ASoulCharacterHero* NewOwner);

	// 卸下武器并移除武器输入
	virtual  void UnequipFromCharacter();
	
	void HandleStanceInput(const FInputActionValue& Value);

	void HandleDrawStowInput(const FInputActionValue& Value);

	// 重新绑定输入，用于角色输入组件初始化较晚的情况
	void BindInputTo(UEnhancedInputComponent* InInputComponent);

	void RefreshLocalInput();

	// 移除输入绑定
	void UnbindInput();

	void GrantWeaponAbility();
	
	void RemoveWeaponAbility();

	virtual const FSoulMovementProfile& GetCurrentMovementProfile() const;


protected:
	UPROPERTY(EditDefaultsOnly, Category="Weapon|Attachment")
	FName EquipSocketName = NAME_None;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon|Input")
	TObjectPtr<UInputAction> StanceAction;

	UPROPERTY(EditDefaultsOnly, Category="Weapon|Ability")
	TSubclassOf<UGameplayAbility> SwitchStanceAbility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon|Input")
	TObjectPtr<UInputAction> DrawStowAction;

	UPROPERTY(EditDefaultsOnly, Category="Weapon|Ability")
	TSubclassOf<UGameplayAbility> DrawStowAbility;
	
	UPROPERTY(EditDefaultsOnly, Category="Weapon|Animation")
	TSubclassOf<UAnimInstance> DefaultAnimLayerClass;
	
	// 武器自己的 Mapping Context
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Input")
	TObjectPtr<UInputMappingContext> WeaponMappingContext;
	
	// 武器 IMC 优先级
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Input")
	int32 MappingPriority = 10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category = "Weapon|Movement")
	FSoulMovementProfile DefaultMovementProfile;

private:
	void AddWeaponMappingContext();
	void RemoveWeaponMappingContext();
	
	FGameplayAbilitySpecHandle SwitchStanceAbilityHandle;

	FGameplayAbilitySpecHandle DrawStowAbilityHandle;
	
	TWeakObjectPtr<ASoulCharacterHero> EquippedHero;

	int32 StanceInputBindingHandle = INDEX_NONE;
	
	int32 DrawStowInputBindingHandle = INDEX_NONE;


	TWeakObjectPtr<UEnhancedInputComponent> BoundInputComponent;
};