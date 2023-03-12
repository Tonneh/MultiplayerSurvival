// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemContainer.h"
#include "Hotbar.generated.h"

enum class EItemType : uint8;
/**
 * 
 */
UCLASS()
class MULTIPLAYERSURVIVAL_API UHotbar : public UItemContainer
{
	GENERATED_BODY()
public:
	bool CheckHotbar(int32 Index, EItemType& ItemType);
protected:
	virtual void HandleOnDrop(UItemContainer* TargetContainer, int32 SourceIndex, int32 TargetIndex) override;
	virtual bool AddItem(FItemInfo Item, int32 TargetIndex, int32 SourceIndex) override;
	virtual void RemoveItemAtIndex(int32 Index) override;
private:
public:
};
