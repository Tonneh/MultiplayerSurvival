// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FirstPersonEquipable.h"
#include "FirstPersonHatchet.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSURVIVAL_API AFirstPersonHatchet : public AFirstPersonEquipable
{
	GENERATED_BODY()

public:
	AFirstPersonHatchet();

private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

public:
};
