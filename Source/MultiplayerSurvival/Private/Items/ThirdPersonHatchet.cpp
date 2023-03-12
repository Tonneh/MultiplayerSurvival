// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ThirdPersonHatchet.h"

#include "Character/SurvivalCharacter.h"
#include "Components/ArrowComponent.h"
#include "HarvestingSystem/LargeItemMaster.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Sound/SoundCue.h"


AThirdPersonHatchet::AThirdPersonHatchet()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->bOwnerNoSee = true;
}

void AThirdPersonHatchet::UseEquippable()
{
	if (!bIsSwinging)
	{
		SurvivalCharacter = SurvivalCharacter == nullptr ? Cast<ASurvivalCharacter>(GetOwner()) : SurvivalCharacter;
		if (SurvivalCharacter && ThirdPersonChoppingAnimMontage && FirstPersonChoppingAnimMontage)
		{
			bIsSwinging = true;
			SurvivalCharacter->ClientPlayFirstPersonMontage(FirstPersonChoppingAnimMontage);
			SurvivalCharacter->MulticastPlayThirdPersonMontage(ThirdPersonChoppingAnimMontage);
		}
	}
}

void AThirdPersonHatchet::EndAnim()
{
	bIsSwinging = false;
	if (SurvivalCharacter)
	{
		SurvivalCharacter->bCanSwap = true; 
	}
}

void AThirdPersonHatchet::ServerOnOverlap_Implementation(FVector Location, FRotator Rotation)
{
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel2));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	TArray<AActor*> HitActors;
	TArray<AActor*> IgnoreActors = {GetOwner()}; 
	SurvivalCharacter = SurvivalCharacter == nullptr ? Cast<ASurvivalCharacter>(GetOwner()) : SurvivalCharacter;
	if (SurvivalCharacter && UKismetSystemLibrary::SphereOverlapActors(this, Location, 30.f, ObjectTypes,
	                                                                   AActor::StaticClass(),
	                                                                   IgnoreActors, HitActors))
	{
		for (const auto HitActor : HitActors)
		{
			if (ALargeItemMaster* Actor = Cast<ALargeItemMaster> (HitActor))
			{
				Actor->MulticastPlayDamageEffect(Location, Rotation); 
			}
			else if (HitActor->IsA(APawn::StaticClass()))
			{
				// can play something here, this is if its another player 
			}
			if (HasAuthority())
			{
				UGameplayStatics::ApplyDamage(HitActor, GetDamage(), SurvivalCharacter->GetInstigatorController(),
							  SurvivalCharacter,
							  UDamageType::StaticClass());
			}
		}
		//DrawDebugSphere(GetWorld(), Location, 30, 12, FColor::Red, false, 10.f);
	}
}

void AThirdPersonHatchet::ClientGetCharacterArrowRotation_Implementation()
{
	SurvivalCharacter = SurvivalCharacter == nullptr ? Cast<ASurvivalCharacter>(GetOwner()) : SurvivalCharacter;
	if (SurvivalCharacter)
	{
		ServerOnOverlap(SurvivalCharacter->GetArrow()->GetComponentLocation(),
		                SurvivalCharacter->GetArrow()->GetComponentRotation());
	}
}
