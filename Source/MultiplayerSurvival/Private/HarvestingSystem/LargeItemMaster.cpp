// Fill out your copyright notice in the Description page of Project Settings.
#include "HarvestingSystem/LargeItemMaster.h"

#include "Character/SurvivalCharacter.h"
#include "HarvestingSystem/DestructableHarvestable.h"
#include "Items/ThirdPersonEquipable.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Sound/SoundCue.h"


ALargeItemMaster::ALargeItemMaster()
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	Health = 100.f;
	bReplicates = true;
}

void ALargeItemMaster::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                                     AController* InstigatorController, AActor* DamageCauser)
{
	if (DamagedActor)
	{
		const float LocalHealth = Health - Damage;
		if (LocalHealth <= 0)
		{
			if (const FLargeItem* Item = DataTable->FindRow<FLargeItem>(
				FName(UKismetSystemLibrary::GetClassDisplayName(GetClass())), "", true))
			{
				const FTransform LocalTransform = GetActorTransform();
				Destroy();
				ADestructableHarvestable* DestructableActor = GetWorld()->SpawnActorDeferred<ADestructableHarvestable>(
					Item->Class, LocalTransform);
				if (DestructableActor)
				{
					DestructableActor->Direction = DamageCauser->GetActorForwardVector();
					UGameplayStatics::FinishSpawningActor(DestructableActor, LocalTransform);
				}
			}
		}
		Health = LocalHealth;
		if (DataTable)
		{
			if (const FLargeItem* Item = DataTable->FindRow<FLargeItem>(
				FName(UKismetSystemLibrary::GetClassDisplayName(GetClass())), "", true))
			{
				TArray<FResource> Resources = Item->Resource;
				for (FResource Resource : Resources)
				{
					if (ASurvivalCharacter* Char = Cast<ASurvivalCharacter>(DamageCauser))
					{
						float ToolTypeMultiplier = Char->GetThirdPersonEquippedItem()->GetToolType() == Resource.
						                           PreferredToolType
							                           ? FMath::RandRange(0.1f, 0.2f)
							                           : FMath::RandRange(0.05f, 0.1f);
						float ToolTierMultiplier = Char->GetThirdPersonEquippedItem()->GetToolTier() ==
						                           EToolTier::EToolTier_Iron
							                           ? FMath::RandRange(1.2f, 1.6f)
							                           : FMath::RandRange(0.8f, 1.2f);
						float ResourceAmount = HarvestFoliageMath(Resource.Quantity, 1.0f, ToolTypeMultiplier,
						                                          ToolTierMultiplier,
						                                          Char->GetThirdPersonEquippedItem()->GetDamage());
						Resource.Quantity = FMath::TruncToInt(ResourceAmount);
						if (Resource.Quantity >= 1)
						{
							Char->ServerHarvestItem(Resource);
						}
					}
				}
			}
		}
	}
}

void ALargeItemMaster::MulticastPlayDamageEffect_Implementation(FVector Location, FRotator Rotation)
{
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, Location, UKismetMathLibrary::GetForwardVector(Rotation) * 100.f,
										 ECC_Visibility);
	if (HitFX && HitSoundFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, HitFX, Location);
		UGameplayStatics::PlaySoundAtLocation(this, HitSoundFX, Location); 
	}
}

void ALargeItemMaster::BeginPlay()
{
	Super::BeginPlay();
	DataTable = LoadObject<UDataTable>(nullptr, UTF8_TO_TCHAR(
		                                   "/Script/Engine.DataTable'/Game/Blueprints/DataTables/DT_LargeItemResource.DT_LargeItemResource'"));
	DataTable->AddToRoot();

	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ALargeItemMaster::ReceiveDamage);
	}
}

float ALargeItemMaster::HarvestFoliageMath(float BaseVar, float RateVar, float ToolTypeVar, float ToolTierVar,
                                           float DamageVar)
{
	return BaseVar * RateVar * ToolTypeVar * ToolTierVar * DamageVar;
}
