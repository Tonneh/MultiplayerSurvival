// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemContainer.generated.h"


struct FItem;
class ASurvivalCharacter;
class ASurvivalPlayerController;
enum class EContainerType : uint8;
struct FItemInfo;
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MULTIPLAYERSURVIVAL_API UItemContainer : public UActorComponent
{
	GENERATED_BODY()

public:
	UItemContainer();
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
protected:
	UPROPERTY()
	ASurvivalCharacter* SurvivalCharacter;
	UPROPERTY()
	ASurvivalPlayerController* SurvivalPlayerController;
	
public:
	
	/****************************************************************************/
	/*Add, Removing Items														*/
	/****************************************************************************/
	
	UFUNCTION(Server, Reliable)
	void ServerOnSlotDrop(UItemContainer* TargetContainer, int32 SourceIndex, int32 TargetIndex);

	void TransferItem(UItemContainer* TargetContainer, int32 TargetIndex, int32 SourceIndex);

	void UpdateUI(int32 Index, bool ResetSlot, FItemInfo ItemInfo);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerAddItem(FItemInfo item);

protected:
	
	virtual void HandleOnDrop(UItemContainer* TargetContainer, int32 SourceIndex, int32 TargetIndex);
	virtual bool AddItem(FItemInfo Item, int32 TargetIndex, int32 SourceIndex);
	virtual void RemoveItemAtIndex(int32 Index);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TArray<FItemInfo> ItemsArray;

private:
	int32 FindEmptySlot();
	void AddItem(FItemInfo Item);
	bool HasItemToStack(FItemInfo Item);
	
	UPROPERTY(EditDefaultsOnly)
	EContainerType ContainerType;
	
	/****************************************************************************/
	/*Crafting																	*/
	/****************************************************************************/

public:
	bool ContainsItems(TArray<FItem> Items);
	void RemoveItems(TArray<FItem> ItemsToRemove);
	TArray<int32> GetIndexesOfItems(int32 ItemID);

	/****************************************************************************/
	/*Consumables																*/
	/****************************************************************************/

	/* Returns true if item is equal to zero and removed */ 
	bool UpdateItemQuantity(int32 Index, int32 Quantity);

	/* If Item is removed returns true */ 
	bool RemoveQuantity(int32 Index, int32 AmountToRemove); 
public:
	void printArray();
	FItemInfo GetItemAtIndex(int32 Index) const;
	FORCEINLINE EContainerType GetContainerType() const { return ContainerType; }
	FORCEINLINE ASurvivalPlayerController* GetSurvivalPlayerController() const { return SurvivalPlayerController; }
};
