// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SurvivalPlayerController.generated.h"

enum class EPlayerStats : uint8;
enum class EArmorType : uint8;
struct FItemInfo;
enum class EContainerType : uint8;
class UMainWidget;
class UUserWidget;

UCLASS()
class MULTIPLAYERSURVIVAL_API ASurvivalPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	/****************************************************************************/
	/*Inventory																	*/
	/****************************************************************************/
	
	UFUNCTION(Client, Reliable)
	void ClientOpenInventory();

	UFUNCTION(BlueprintImplementableEvent)
	void SetInventoryVis(bool Visibility);

	UFUNCTION(Client, Reliable)
	void ClientUpdateItemSlot(EContainerType Container, int32 Index, FItemInfo ItemInfo);
	UFUNCTION(Client, Reliable)
	void ClientResetItemSlot(EContainerType Container, int32 Index);
	UFUNCTION(Client, Reliable)
	void ClientShowHarvestedItemWidget(UTexture2D* ResourceImage, int32 ResourceQuantity, FName ResourceName);

	UFUNCTION(BlueprintImplementableEvent)
	void BPUpdateItemSlot(EContainerType Container, int32 Index, FItemInfo ItemInfo);
	UFUNCTION(BlueprintImplementableEvent)
	void BPResetItemSlot(EContainerType Container, int32 Index);
	UFUNCTION(BlueprintImplementableEvent)
	void BPShowHarvestedItemWidget(UTexture2D* ResourceImage, int32 ResourceQuantity, FName ResourceName);
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	TSubclassOf<UUserWidget> MainWidgetClass;

	UPROPERTY()
	UMainWidget* MainWidget;

	bool bIsInventoryShown = false;

	/****************************************************************************/
	/*Crafting																	*/
	/****************************************************************************/
public: 
	UFUNCTION(Client, Reliable)
	void ClientUpdateCraftStatus(bool CanCraft);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void BPUpdateCraftStatus(bool CanCraft); 

	UFUNCTION(Client, Reliable)
	void ClientShowProgressBar();

	UFUNCTION(BlueprintImplementableEvent)
	void BPShowProgressBar();

	/****************************************************************************/
	/*PlayerStat																*/
	/****************************************************************************/
	
	UFUNCTION(Client, Reliable)
	void ClientUpdateStatBar(EPlayerStats Stat, float Curr, float Max);

	UFUNCTION(BlueprintImplementableEvent)
	void BPUpdateStatBar(EPlayerStats Stat, float Curr, float Max);

	UFUNCTION(Client, Reliable)
	void ClientShowOrHideStarving(bool ShowOrHide);

	UFUNCTION(BlueprintImplementableEvent)
	void ShowOrHideStarving(bool ShowOrHide);
	
	UFUNCTION(Client, Reliable)
	void ClientShowOrHideDehydration(bool ShowOrHide);

	UFUNCTION(BlueprintImplementableEvent)
	void ShowOrHideDehydration(bool ShowOrHide);

public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE UMainWidget* GetMainWidget() const { return MainWidget; }
};
