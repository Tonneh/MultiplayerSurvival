// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Hotbar.h"

#include "Character/SurvivalCharacter.h"
#include "Inventory/ItemContainer.h"
#include "MultiplayerSurvival/Types.h"

bool UHotbar::CheckHotbar(int32 Index, EItemType& ItemType)
{
	if (ItemsArray[Index].ItemID != 0)
	{
		ItemType = ItemsArray[Index].ItemType;
		return true;
	}
	return false;
}

void UHotbar::HandleOnDrop(UItemContainer* TargetContainer, int32 SourceIndex, int32 TargetIndex)
{
	if (TargetContainer == nullptr)
	{
		return;
	}
	switch (TargetContainer->GetContainerType())
	{
		case EContainerType::EContainerType_PlayerInventory:
			TransferItem(TargetContainer, TargetIndex, SourceIndex);
			break;
		case EContainerType::EContainerType_PlayerHotbar:
			TransferItem(TargetContainer, TargetIndex, SourceIndex);
		default:
			break;
	}
}

bool UHotbar::AddItem(FItemInfo Item, int32 TargetIndex, int32 SourceIndex)
{
	const bool b = Super::AddItem(Item,  TargetIndex, SourceIndex);
	UpdateUI(TargetIndex, false, Item);
	return b;
}

void UHotbar::RemoveItemAtIndex(int32 Index)
{
	Super::RemoveItemAtIndex(Index);
	SurvivalCharacter = SurvivalCharacter == nullptr ? Cast<ASurvivalCharacter>(GetOwner()) : SurvivalCharacter;
	if (SurvivalCharacter)
	{
		SurvivalCharacter->ServerUnequipCurrentItem(Index);
	}
	UpdateUI(Index, true, {});
}
