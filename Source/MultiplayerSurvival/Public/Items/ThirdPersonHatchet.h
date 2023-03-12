// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ThirdPersonEquipable.h"
#include "ThirdPersonHatchet.generated.h"

class USoundCue;
class ASurvivalCharacter;
/**
 * 
 */
UCLASS()
class MULTIPLAYERSURVIVAL_API AThirdPersonHatchet : public AThirdPersonEquipable
{
	GENERATED_BODY()

public:
	AThirdPersonHatchet();

	virtual void UseEquippable() override;
	virtual void EndAnim() override;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	UAnimMontage* ThirdPersonChoppingAnimMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	UAnimMontage* FirstPersonChoppingAnimMontage;

private:
	bool bIsSwinging = false;
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;
	
	UPROPERTY()
	ASurvivalCharacter* SurvivalCharacter;

	UFUNCTION(Server, Reliable)
	void ServerOnOverlap(FVector Location, FRotator Rotation);

public:
	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientGetCharacterArrowRotation();
};
