// Fill out your copyright notice in the Description page of Project Settings.


#include "HarvestingSystem/DestructableHarvestable.h"
#include "Net/UnrealNetwork.h"

ADestructableHarvestable::ADestructableHarvestable()
{
	bReplicates = true;
}

void ADestructableHarvestable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADestructableHarvestable, Direction);
}
