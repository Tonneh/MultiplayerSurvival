// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/ItemContainer.h"

#include "IDetailTreeNode.h"
#include "Character/SurvivalCharacter.h"
#include "Inventory/Hotbar.h"
#include "Items/EquipableMaster.h"
#include "Kismet/KismetMathLibrary.h"
#include "MultiplayerSurvival/Types.h"
#include "PlayerController/SurvivalPlayerController.h"

UItemContainer::UItemContainer()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UItemContainer::BeginPlay()
{
	Super::BeginPlay();

	ItemsArray.SetNumZeroed(300);
	if (SurvivalCharacter)
	{
		SurvivalPlayerController = SurvivalPlayerController == nullptr
			                           ? Cast<ASurvivalPlayerController>(SurvivalCharacter->GetController())
			                           : SurvivalPlayerController;
	}
}

void UItemContainer::ServerOnSlotDrop_Implementation(UItemContainer* SourceContainer, int32 SourceIndex,
                                                     int32 TargetIndex)
{
	HandleOnDrop(SourceContainer, SourceIndex, TargetIndex);
}

void UItemContainer::HandleOnDrop(UItemContainer* TargetContainer, int32 SourceIndex, int32 TargetIndex)
{
}

int32 UItemContainer::FindEmptySlot()
{
	for (int i = 0; i < ItemsArray.Num(); i++)
	{
		if (ItemsArray[i].ItemID == 0)
		{
			return i;
		}
	}
	return -1;
}

void UItemContainer::UpdateUI(int32 Index, bool ResetSlot, FItemInfo ItemInfo)
{
	SurvivalCharacter = SurvivalCharacter == nullptr ? Cast<ASurvivalCharacter>(GetOwner()) : SurvivalCharacter;
	if (SurvivalCharacter)
	{
		SurvivalPlayerController = SurvivalPlayerController == nullptr
			                           ? Cast<ASurvivalPlayerController>(SurvivalCharacter->GetController())
			                           : SurvivalPlayerController;
		if (SurvivalPlayerController && !ResetSlot)
		{
			SurvivalPlayerController->ClientUpdateItemSlot(ContainerType, Index, ItemInfo);
		}
		else if (SurvivalPlayerController && ResetSlot)
		{
			SurvivalPlayerController->ClientResetItemSlot(ContainerType, Index);
		}
	}
}

FItemInfo UItemContainer::GetItemAtIndex(int32 Index) const
{
	return ItemsArray[Index];
}

void UItemContainer::RemoveItemAtIndex(int32 Index)
{
	ItemsArray[Index].ItemID = 0;
}

bool UItemContainer::ContainsItems(TArray<FItem> Items)
{
	TArray<FItem> HasItems = Items;
	for (FItem RequiredItem : Items)
	{
		bool SameItem = false;
		bool EnoughQuantity = false;
		int32 TotalNeeded = RequiredItem.ItemQuantity;
		for (FItemInfo ItemInInventory : ItemsArray)
		{
			SameItem = RequiredItem.ItemID == ItemInInventory.ItemID;
			EnoughQuantity = TotalNeeded <= ItemInInventory.ItemQuanity;
			if (SameItem && EnoughQuantity)
			{
				for (int HasItem = 0; HasItem < HasItems.Num(); HasItem++)
				{
					if (HasItems[HasItem].ItemID == RequiredItem.ItemID)
					{
						HasItems.RemoveAt(HasItem);
					}
				}
			}
		}
	}
	return HasItems.IsEmpty();
}

void UItemContainer::RemoveItems(TArray<FItem> ItemsToRemove)
{
	for (FItem Item : ItemsToRemove)
	{
		int32 TotalToRemove = Item.ItemQuantity;
		TArray<int32> Indexes = GetIndexesOfItems(Item.ItemID);
		for (int32 Index : Indexes)
		{
			if (ItemsArray[Index].ItemQuanity - TotalToRemove <= 0)
			{
				TotalToRemove -= ItemsArray[Index].ItemQuanity - TotalToRemove;
				ItemsArray[Index].ItemID = 0;
				UpdateUI(Index, true, ItemsArray[Index]);
				if (TotalToRemove == 0)
				{
					break;
				}
			}
			else
			{
				ItemsArray[Index].ItemQuanity -= TotalToRemove;
				UpdateUI(Index, false, ItemsArray[Index]);
				break;
			}
		}
	}
}

TArray<int32> UItemContainer::GetIndexesOfItems(int32 ItemID)
{
	TArray<int32> ReturnArray;
	int32 Index = 0;
	for (FItemInfo ItemInfo : ItemsArray)
	{
		if (ItemInfo.ItemID == ItemID && ItemInfo.ItemQuanity > 0)
		{
			ReturnArray.Add(Index);
		}
		Index++;
	}
	return ReturnArray;
}

bool UItemContainer::UpdateItemQuantity(int32 Index, int32 Quantity)
{
	ItemsArray[Index].ItemQuanity = Quantity;
	if (ItemsArray[Index].ItemQuanity <= 0)
	{
		RemoveItemAtIndex(Index);
		return true; 
	}
	UpdateUI(Index, false, ItemsArray[Index]);
	return false; 
}

bool UItemContainer::RemoveQuantity(int32 Index, int32 AmountToRemove)
{
	return UpdateItemQuantity(Index, ItemsArray[Index].ItemQuanity - AmountToRemove); 
}

void UItemContainer::printArray()
{
	for (auto i : ItemsArray)
	{
		UE_LOG(LogTemp, Warning, TEXT("%d"), i.ItemID);
	}
}

void UItemContainer::TransferItem(UItemContainer* TargetContainer, int32 TargetIndex, int32 SourceIndex)
{
	if (TargetContainer)
	{
		FItemInfo Item = TargetContainer->GetItemAtIndex(TargetIndex);
		const FItemInfo ItemToTransfer = GetItemAtIndex(SourceIndex);
		if (Item.ItemID > 0)
		{
			if (TargetContainer->GetContainerType() == EContainerType::EContainerType_PlayerHotbar)
			{
				if (ItemToTransfer.ItemType == EItemType::EItemType_Resource || ItemToTransfer.ItemType ==
					EItemType::EItemType_Armor || Item.ItemType == EItemType::EItemType_Armor || Item.ItemType ==
					EItemType::EItemType_Resource)
				{
					return;
				}
				if (SurvivalCharacter->GetHotBarIndex() == TargetIndex)
				{
					SurvivalCharacter->ServerUnequipCurrentItem(TargetIndex);
				}
			}
			else if (GetContainerType() == EContainerType::EContainerType_PlayerHotbar)
			{
				if (ItemToTransfer.ItemType == EItemType::EItemType_Resource || ItemToTransfer.ItemType ==
					EItemType::EItemType_Armor || Item.ItemType == EItemType::EItemType_Armor || Item.ItemType ==
					EItemType::EItemType_Resource)
				{
					return;
				}
				if (SurvivalCharacter->GetHotBarIndex() == SourceIndex)
				{
					SurvivalCharacter->ServerUnequipCurrentItem(SourceIndex);
				}
			}
			TargetContainer->ItemsArray[TargetIndex] = ItemToTransfer;
			ItemsArray[SourceIndex] = Item;
			TargetContainer->UpdateUI(TargetIndex, false, ItemToTransfer);
			UpdateUI(SourceIndex, false, Item);
		}
		else
		{
			if (TargetContainer->GetContainerType() == EContainerType::EContainerType_PlayerHotbar && 
				(ItemToTransfer.ItemType == EItemType::EItemType_Resource || ItemToTransfer.ItemType ==
				EItemType::EItemType_Armor))
			{
				return;
			}
			if (TargetContainer->AddItem(ItemToTransfer, TargetIndex, SourceIndex))
			{
				RemoveItemAtIndex(SourceIndex);
			}
		}
	}
}

void UItemContainer::AddItem(FItemInfo Item)
{
	int32 Index = 0;
	if (Item.IsStackable && HasItemToStack(Item))
	{
		int32 CurrentSlotQuantity;
		int32 TempSlotQuantity;
		int32 TotalItemQuantity;
		int32 MaxStackSize;
		MaxStackSize = Item.StackSize;
		for (FItemInfo ItemInArray : ItemsArray)
		{
			bool SameID = ItemInArray.ItemID == Item.ItemID;
			bool EnoughSpace = ItemInArray.ItemQuanity < MaxStackSize;
			if (SameID && EnoughSpace)
			{
				CurrentSlotQuantity = ItemInArray.ItemQuanity;
				TempSlotQuantity = CurrentSlotQuantity;
				TotalItemQuantity = Item.ItemQuanity;
				CurrentSlotQuantity = CurrentSlotQuantity + TotalItemQuantity >= MaxStackSize
					                      ? MaxStackSize
					                      : CurrentSlotQuantity + TotalItemQuantity;
				Item.ItemQuanity = CurrentSlotQuantity;
				ItemsArray[Index] = Item;
				UpdateUI(Index, false, Item);
				TotalItemQuantity = FMath::Clamp(TotalItemQuantity - (MaxStackSize - TempSlotQuantity), 0,
				                                 TotalItemQuantity);
				Item.ItemQuanity = TotalItemQuantity;
			}
			Index++;
		}
		if (Item.ItemQuanity > Item.StackSize && Item.ItemQuanity > 0)
		{
			TotalItemQuantity = Item.ItemQuanity;
			MaxStackSize = Item.StackSize;
			while (TotalItemQuantity > 0)
			{
				const int32 Slot = FindEmptySlot();
				if (Slot != -1)
				{
					CurrentSlotQuantity = 0;
					TempSlotQuantity = CurrentSlotQuantity;
					CurrentSlotQuantity = CurrentSlotQuantity + TotalItemQuantity >= MaxStackSize
						                      ? MaxStackSize
						                      : CurrentSlotQuantity + TotalItemQuantity;
					Item.ItemQuanity = CurrentSlotQuantity;
					ItemsArray[Slot] = Item;
					UpdateUI(Slot, false, Item);
					TotalItemQuantity = FMath::Clamp(TotalItemQuantity - (MaxStackSize - TempSlotQuantity), 0,
					                                 TotalItemQuantity);
				}
			}
		}
		else if (Item.ItemQuanity > 0)
		{
			const int32 Slot = FindEmptySlot();
			if (Slot != -1)
			{
				ItemsArray[Slot] = Item;
				UpdateUI(Slot, false, Item);
			}
		}
		return;
	}
	const int32 Slot = FindEmptySlot();
	if (Slot != -1)
	{
		ItemsArray[Slot] = Item;
		UpdateUI(Slot, false, Item);
	}
}

bool UItemContainer::HasItemToStack(FItemInfo Item)
{
	for (auto ItemInArray : ItemsArray)
	{
		const bool SameItem = ItemInArray.ItemID == Item.ItemID;
		const bool EnoughSpace = ItemInArray.ItemQuanity < ItemInArray.StackSize;
		if (SameItem && EnoughSpace)
		{
			return true;
		}
	}
	return false;
}

bool UItemContainer::AddItem(FItemInfo Item, int32 TargetIndex, int32 SourceIndex)
{
	if (ItemsArray.IsValidIndex(TargetIndex) && ItemsArray[TargetIndex].ItemID == 0)
	{
		ItemsArray[TargetIndex] = Item;
		return true;
	}
	return false;
}

void UItemContainer::ServerAddItem_Implementation(FItemInfo item)
{
	AddItem(item);
}

void UItemContainer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
