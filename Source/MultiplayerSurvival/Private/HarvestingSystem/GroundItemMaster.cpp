// Fill out your copyright notice in the Description page of Project Settings.


#include "HarvestingSystem/GroundItemMaster.h"

#include "Character/SurvivalCharacter.h"
#include "HarvestingSystem/DestructableHarvestable.h"
#include "HarvestingSystem/LargeItemMaster.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MultiplayerSurvival/Types.h"

AGroundItemMaster::AGroundItemMaster()
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
}

void AGroundItemMaster::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                                      AController* InstigatorController, AActor* DamageCauser)
{
	float LocalHealth = Health - Damage;
	if (LocalHealth <= 0.f)
	{
		if (const FLargeItem* Item = DataTable->FindRow<FLargeItem>(
			FName(UKismetSystemLibrary::GetClassDisplayName(GetClass())), "", true))
		{
			if (Item)
			{
				const FTransform LocalTransform = GetActorTransform();
				Destroy();
				GetWorld()->SpawnActor<ADestructableHarvestable>(Item->Class, LocalTransform);
			}
		}
	}
	Health = LocalHealth;
	if (DataTable)
	{
		if (const FLargeItem* Item = DataTable->FindRow<FLargeItem>(
			FName(UKismetSystemLibrary::GetClassDisplayName(GetClass())), "", true))
		{
			if (Item)
			{
				for (FResource Resource : Item->Resource)
				{
					if (ASurvivalCharacter* Char = Cast<ASurvivalCharacter>(DamageCauser))
					{
						const float HarvestAmount = HarvestFoliageMath(Resource.Quantity, 1.0f,
						                                               FMath::RandRange(1.2f, 1.8f));
						Resource.Quantity = FMath::TruncToInt(HarvestAmount);
						if (HarvestAmount >= 1)
						{
							Char->ServerHarvestItem(Resource);
						}
					}
				}
			}
		}
	}
}

void AGroundItemMaster::BeginPlay()
{
	Super::BeginPlay();
	DataTable = LoadObject<UDataTable>(nullptr, UTF8_TO_TCHAR(
		                                   "/Script/Engine.DataTable'/Game/Blueprints/DataTables/DT_GroundResources.DT_GroundResources'"));
	DataTable->AddToRoot();
	OnTakeAnyDamage.AddDynamic(this, &AGroundItemMaster::ReceiveDamage);
}

float AGroundItemMaster::HarvestFoliageMath(float BaseVar, float RateVar, float Rand)
{
	return BaseVar * RateVar * Rand;
}
