// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/PlayerInventory.h"
#include "Inventory/ItemContainer.h"
#include "MultiplayerSurvival/Types.h"

void UPlayerInventory::HandleOnDrop(UItemContainer* TargetContainer, int32 SourceIndex, int32 TargetIndex)
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
			break;
		default:
			break;
	}
}

bool UPlayerInventory::AddItem(FItemInfo Item, int32 TargetIndex, int32 SourceIndex)
{
	const bool b = Super::AddItem(Item, TargetIndex, SourceIndex);
	UpdateUI(TargetIndex, false, Item); 
	return b;
}

void UPlayerInventory::RemoveItemAtIndex(int32 Index)
{
	Super::RemoveItemAtIndex(Index);
	UpdateUI(Index, true, {});
}
