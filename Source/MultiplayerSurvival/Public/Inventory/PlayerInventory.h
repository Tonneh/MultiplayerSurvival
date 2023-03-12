// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemContainer.h"
#include "PlayerInventory.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSURVIVAL_API UPlayerInventory : public UItemContainer
{
	GENERATED_BODY()
protected:
	virtual void HandleOnDrop(UItemContainer* TargetContainer, int32 SourceIndex, int32 TargetIndex) override;
	virtual bool AddItem(FItemInfo Item, int32 TargetIndex, int32 SourceIndex) override;
	virtual void RemoveItemAtIndex(int32 Index) override;
private:
};
