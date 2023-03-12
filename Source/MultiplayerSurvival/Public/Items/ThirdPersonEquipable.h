// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EquipableMaster.h"
#include "ThirdPersonEquipable.generated.h"

enum class EToolTier : uint8;
enum class EHarvestingToolType : uint8;
/**
 * 
 */
UCLASS()
class MULTIPLAYERSURVIVAL_API AThirdPersonEquipable : public AEquipableMaster
{
	GENERATED_BODY()
public:

protected:
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* EquipWeaponMontage;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* UnequipWeaponMontage;

private:
	UPROPERTY(EditDefaultsOnly)
	EHarvestingToolType ToolType;
	UPROPERTY(EditDefaultsOnly)
	EToolTier ToolTier;

	UPROPERTY(EditDefaultsOnly)
	float Damage;
public:
	FORCEINLINE UAnimMontage* GetEquipAnim() const { return EquipWeaponMontage; }
	FORCEINLINE UAnimMontage* GetUnequipAnim() const { return UnequipWeaponMontage; }
	FORCEINLINE EHarvestingToolType GetToolType() { return ToolType; }
	FORCEINLINE EToolTier GetToolTier() { return ToolTier; }
	FORCEINLINE float GetDamage() const { return Damage; }
};
