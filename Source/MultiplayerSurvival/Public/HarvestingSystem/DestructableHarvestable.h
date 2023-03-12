// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DestructableHarvestable.generated.h"


class UCapsuleComponent;
UCLASS()
class MULTIPLAYERSURVIVAL_API ADestructableHarvestable : public AActor
{
	GENERATED_BODY()

public:
	ADestructableHarvestable();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated, BlueprintReadOnly)
	FVector Direction;

protected:
private:
public:
};
