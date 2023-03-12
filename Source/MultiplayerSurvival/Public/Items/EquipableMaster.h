// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemMaster.h"
#include "MultiplayerSurvival/Types.h"
#include "EquipableMaster.generated.h"


UCLASS()
class MULTIPLAYERSURVIVAL_API AEquipableMaster : public AItemMaster
{
	GENERATED_BODY()
public:
	virtual void UseEquippable();
	virtual void EndAnim();
	virtual void Destroy(); 
protected:
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Equippable", meta = (AllowPrivateAccess))
	FEquipableInfo EquipableInfo;
public:
	FORCEINLINE FEquipableInfo GetEquipableInfo() const { return EquipableInfo; }
};
