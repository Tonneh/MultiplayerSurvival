// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EquipableMaster.h"
#include "FirstPersonEquipable.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSURVIVAL_API AFirstPersonEquipable : public AEquipableMaster
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* EquipWeaponMontage;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* UnequipWeaponMontage;
public:
	FORCEINLINE UAnimMontage* GetEquipAnim() const { return EquipWeaponMontage; }
	FORCEINLINE UAnimMontage* GetUnequipAnim() const { return UnequipWeaponMontage; }
};
