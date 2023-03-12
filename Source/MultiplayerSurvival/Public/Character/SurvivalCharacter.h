// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MultiplayerSurvival/Types.h"
#include "SurvivalCharacter.generated.h"


struct FPlayerStats;
enum class ECraftingType : uint8;
class USoundCue;
class AGroundItemMaster;
class UDataTable;
struct FResource;
class AFirstPersonEquipable;
struct FItemInfo;
class AThirdPersonEquipable;
enum class EEquipableState : uint8;
class UHotbar;
enum class EArmorType : uint8;
enum class EContainerType : uint8;
class ASurvivalPlayerController;
class UPlayerInventory;
class UCameraComponent;

UCLASS()
class MULTIPLAYERSURVIVAL_API ASurvivalCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Default 
	ASurvivalCharacter();
private:
	UPROPERTY()
	ASurvivalPlayerController* SurvivalPlayerController;

	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
	USkeletalMeshComponent* FirstPersonMesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCamera;

	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
	USkeletalMeshComponent* ThirdPersonMesh;

	UPROPERTY(VisibleDefaultsOnly, Category = "Arrow")
	UArrowComponent* SurvivalCharacterArrow;

	UPROPERTY(EditDefaultsOnly)
	UDataTable* ItemDataTable;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PostInitializeComponents() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/****************************************************************************/
	/*Input Movement															*/
	/****************************************************************************/

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	virtual void Jump() override;

	UFUNCTION(Server, Reliable)
	void ServerLeftMouseClicked();

	/****************************************************************************/
	/*Inventory																	*/
	/****************************************************************************/

	UPROPERTY(VisibleDefaultsOnly, Category = "Inventory", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPlayerInventory* Inventory;

	UPROPERTY(VisibleDefaultsOnly, Category = "Inventory", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UHotbar* Hotbar;

	bool bInventoryOpened = false;

	void OpenInventory();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerHotbarInput(const int32 Index);

	void UseHotbar(const int32 Index);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerOnSlotDrop(EContainerType TargetContainer, EContainerType SourceContainer, int32 TargetIndex,
	                      int32 SourceIndex, EArmorType ArmorType);

	/****************************************************************************/
	/*Equipables																*/
	/****************************************************************************/
	UPROPERTY()
	AThirdPersonEquipable* ThirdPersonEquippedItem;

	UPROPERTY()
	AFirstPersonEquipable* FirstPersonEquipable;

	// Index that was selected in hotbar
	int32 HotbarIndex;
	// The index where we're currently holding the item 
	int32 EquippedIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	EEquipableState EquipableState;

	UFUNCTION(Server, Reliable)
	void ServerSpawnEquipableThirdPerson(TSubclassOf<AActor> EquipableClass, const FItemInfo ItemInfo,
	                                     const int32 ItemIndex);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastWeaponEquip(AActor* Target, const FName SocketName, const EEquipableState EquippedState);

	UFUNCTION(Client, Reliable)
	void ClientSpawnEquipableFirstPerson(TSubclassOf<AActor> EquipableClass, const FName SocketName);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastUnequipWeaponThirdPerson();

	UFUNCTION(Client, Reliable)
	void ClientUnequipWeaponFirstPerson();

public:
	UFUNCTION(Server, Reliable)
	void ServerUnequipCurrentItem(int32 Index);

	/****************************************************************************/
	/*Animations																*/
	/****************************************************************************/

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayThirdPersonMontage(UAnimMontage* Animation);
	UFUNCTION(Client, Reliable)
	void ClientPlayFirstPersonMontage(UAnimMontage* Animation);

	bool bCanSwap = true;

private:
	FTimerHandle AnimTimerHandle;
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* ThirdPersonPickupMontage;
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* FirstPersonPickupMontage;

public:
	/****************************************************************************/
	/*HarvestSystem																*/
	/****************************************************************************/

	UFUNCTION(Server, Reliable)
	void ServerHarvestItem(FResource Resource);
private:
	void HarvestItem(FResource Resource);

	UFUNCTION(Server, Reliable)
	void ServerInteract();

	UFUNCTION(Server, Reliable)
	void ServerOverlapGroundItems();

	void HarvestGroundItem(AActor* HarvestedActor);

	void ResetHarvestGround();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastHarvestGroundAnim();

	bool bIsHarvesting = false;

	UPROPERTY(EditDefaultsOnly)
	USoundCue* HarvestBushSound;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* HarvestBushFX;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastBushFX();

	/****************************************************************************/
	/*Crafting																	*/
	/****************************************************************************/

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerCheckIfCanCraftItem(int32 ItemID, EContainerType ContainerType, ECraftingType TableType);

	bool CanCraftItem(int32 ItemID, EContainerType ContainerType, ECraftingType TableType);

	UPROPERTY(EditDefaultsOnly)
	UDataTable* CraftingRecipesDataTable;

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerCraftItemOnServer(int32 ItemID, EContainerType ContainerType, ECraftingType TableType);

	bool CraftItem(int32 ItemID, EContainerType ContainerType, ECraftingType TableType);

	bool bIsCrafting = false;

	FTimerHandle CraftingTimerHandle;

	float CraftingTime = 2.5f;

	UFUNCTION()
	void AddCraftedItem(FName ItemIDToAdd, EContainerType ContainerType);

	/****************************************************************************/
	/*PlayerStats																*/
	/****************************************************************************/

	UPROPERTY(EditDefaultsOnly, Category = "Player Stats")
	float CurrHealth;
	UPROPERTY(EditDefaultsOnly, Category = "Player Stats")
	float MaxHealth;
	UPROPERTY(EditDefaultsOnly, Category = "Player Stats")
	float CurrFood;
	UPROPERTY(EditDefaultsOnly, Category = "Player Stats")
	float MaxFood;
	UPROPERTY(EditDefaultsOnly, Category = "Player Stats")
	float CurrWater;
	UPROPERTY(EditDefaultsOnly, Category = "Player Stats")
	float MaxWater;
	UPROPERTY(EditDefaultsOnly, Category = "Player Stats")
	float CurrStam;
	UPROPERTY(EditDefaultsOnly, Category = "Player Stats")
	float MaxStam;
	UPROPERTY(EditDefaultsOnly, Category = "Player Stats")
	int32 CurrLevel;
	UPROPERTY(EditDefaultsOnly, Category = "Player Stats")
	int32 CurrXP;
	UPROPERTY(EditDefaultsOnly, Category = "Player Stats")
	int32 SkillPoints;

	FPlayerStats PlayerStats; 
	
	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
				   AController* InstigatorController, AActor* DamageCauser);
    
    UFUNCTION(Server, Reliable)
    void ServerDecreaseStatsOverTime();

	void FoodWaterDrain(); 

	float DecreaseFloat(float FloatToDecrease, float Percentage, float Max);

	UPROPERTY(EditDefaultsOnly) 
	float FoodWaterDrainSpeed;

	bool bIsStarving = false;

	bool bIsDehydrated = false;
	
	FTimerHandle StatDrainTimer;

	/* Drain health when no water, or food, or both */
	UFUNCTION(Server, Reliable)
	void ServerDecreaseHealthOverTime();

	void DecreaseHealth();

	bool bIsDead = false;

	FTimerHandle HealthDrainTimer;

	/* Drain Stam */

	UFUNCTION(Server, Reliable)
	void ServerDrainStam();

	UFUNCTION(Server, Reliable)
	void ServerRegenStam(); 

	bool bShouldDrainStam = false;

	void StartDrainTimer();

	void DrainStam();

	FTimerHandle StamDrainTimer;

	void StartStamRegenTimer();

	void RegenStam();

	FTimerHandle StamRegenTimer;

	bool bDoOnceStam = true;
	
	bool bDoOnceRegen = true;

	bool bHaveStam = true;

	/****************************************************************************/
	/*Sprint																	*/
	/****************************************************************************/

	void Sprint();

	UFUNCTION(Server, Reliable) 
	void ServerSprint();

	bool bIsSprinting = false;

	void StopSprint();

	UFUNCTION(Server, Reliable)
	void ServerStopSprint();

	/****************************************************************************/
	/*Consumables																*/
	/****************************************************************************/
	
	void ConsumeItem(int32 ItemIndex, EContainerType ContainerType);

	void InstantUpdateStat(EPlayerStats StatToChange, float Amount); 
public: // getters and setters
	FORCEINLINE int32 GetHotBarIndex() const { return HotbarIndex; }
	FORCEINLINE UArrowComponent* GetArrow() const { return SurvivalCharacterArrow; }
	FORCEINLINE UCameraComponent* GetCamera() const { return FirstPersonCamera; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE AThirdPersonEquipable* GetThirdPersonEquippedItem() const { return ThirdPersonEquippedItem; }
	FORCEINLINE ASurvivalPlayerController* GetSurvivalPlayerController() const { return SurvivalPlayerController; }
};
