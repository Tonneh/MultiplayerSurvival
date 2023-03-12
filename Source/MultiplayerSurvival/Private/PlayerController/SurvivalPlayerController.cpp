// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/SurvivalPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "MultiplayerSurvival/Types.h"
#include "Widgets/MainWidget.h"

void ASurvivalPlayerController::ClientUpdateItemSlot_Implementation(EContainerType Container, int32 Index,
                                                                    FItemInfo ItemInfo)
{
	BPUpdateItemSlot(Container, Index, ItemInfo);
}

void ASurvivalPlayerController::ClientResetItemSlot_Implementation(EContainerType Container, int32 Index)
{
	BPResetItemSlot(Container, Index);
}

void ASurvivalPlayerController::ClientShowHarvestedItemWidget_Implementation(UTexture2D* ResourceImage,
                                                                             int32 ResourceQuantity, FName ResourceName)
{
	BPShowHarvestedItemWidget(ResourceImage, ResourceQuantity, ResourceName);
}

void ASurvivalPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		if (MainWidgetClass)
		{
			MainWidget = Cast<UMainWidget>(CreateWidget(this, MainWidgetClass));
			MainWidget->AddToViewport();
		}
	}
}

void ASurvivalPlayerController::ClientShowOrHideStarving_Implementation(bool ShowOrHide)
{
	ShowOrHideStarving(ShowOrHide);
}

void ASurvivalPlayerController::ClientShowOrHideDehydration_Implementation(bool ShowOrHide)
{
	ShowOrHideDehydration(ShowOrHide);
}

void ASurvivalPlayerController::ClientUpdateStatBar_Implementation(EPlayerStats Stat, float Curr, float Max)
{
	BPUpdateStatBar(Stat, Curr, Max); 
}

void ASurvivalPlayerController::ClientShowProgressBar_Implementation()
{
	BPShowProgressBar();
}

void ASurvivalPlayerController::ClientUpdateCraftStatus_Implementation(bool CanCraft)
{
	BPUpdateCraftStatus(CanCraft); 
}


void ASurvivalPlayerController::ClientOpenInventory_Implementation()
{
	bIsInventoryShown = !bIsInventoryShown;
	SetInventoryVis(bIsInventoryShown);
}
