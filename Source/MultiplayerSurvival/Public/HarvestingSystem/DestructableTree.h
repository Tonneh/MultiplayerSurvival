// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DestructableHarvestable.h"
#include "DestructableTree.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSURVIVAL_API ADestructableTree : public ADestructableHarvestable
{
	GENERATED_BODY()
public:
	ADestructableTree();
protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent, meta =(AllowPrivateAccess))
	void AddForce();
private:
public:
};
