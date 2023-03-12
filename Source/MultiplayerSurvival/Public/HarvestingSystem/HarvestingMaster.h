// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HarvestingMaster.generated.h"

UCLASS()
class MULTIPLAYERSURVIVAL_API AHarvestingMaster : public AActor
{
	GENERATED_BODY()

public:
	AHarvestingMaster();

protected:
	virtual void BeginPlay() override;

public:
};
