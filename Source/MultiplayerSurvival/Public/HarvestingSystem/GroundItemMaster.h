// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HarvestingMaster.h"
#include "GroundItemMaster.generated.h"

class UDataTable;
/**
 * 
 */
UCLASS()
class MULTIPLAYERSURVIVAL_API AGroundItemMaster : public AHarvestingMaster
{
	GENERATED_BODY()
public:
	AGroundItemMaster();
	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	                   AController* InstigatorController, AActor* DamageCauser);
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	float Health = 50.f;
private:
	static float HarvestFoliageMath(float BaseVar = 0.0, float RateVar = 0.0, float Rand = 0.0);

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY()
	UDataTable* DataTable;

public:
};
