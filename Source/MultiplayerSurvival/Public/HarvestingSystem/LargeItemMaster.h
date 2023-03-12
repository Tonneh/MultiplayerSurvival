// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HarvestingMaster.h"
#include "Engine/DataTable.h"
#include "LargeItemMaster.generated.h"

class USoundCue;
UCLASS()
class MULTIPLAYERSURVIVAL_API ALargeItemMaster : public AHarvestingMaster
{
	GENERATED_BODY()
public:
	ALargeItemMaster();

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	                   AController* InstigatorController, AActor* DamageCauser);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayDamageEffect(FVector Location, FRotator Rotation);
private: 
	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* HitFX;

	UPROPERTY(EditDefaultsOnly)
	USoundCue* HitSoundFX;
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UDataTable* DataTable;
	
private:
	/* Calculate how much resources a player should recieve */ 
	static float HarvestFoliageMath(float BaseVar = 0.0, float RateVar = 0.0, float ToolTypeVar = 0.0,
	                                float ToolTierVar = 0.0, float DamageVar = 0.0);

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMesh;

	
public:
	FORCEINLINE UStaticMeshComponent* GetStaticMesh() { return StaticMesh; }
};
