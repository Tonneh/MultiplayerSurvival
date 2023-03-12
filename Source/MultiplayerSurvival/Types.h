#pragma once
#include "Engine/DataTable.h"
#include "Types.generated.h"

class AEquipableMaster;
class AItemMaster;
class ADestructableHarvestable;
UENUM(BlueprintType)
enum class EArmorType : uint8
{
	EArmorType_Helmet UMETA(DisplayName = "Helmet"),
	EArmorType_Chest UMETA(DisplayName = "Chest"),
	EArmorType_Legs UMETA(DisplayName = "Legs"),
	EArmorType_Boots UMETA(DisplayName = "Boots"),
};

UENUM(BlueprintType)
enum class EContainerType : uint8
{
	EContainerType_PlayerInventory UMETA(DisplayName = "PlayerInventory"),
	EContainerType_PlayerHotbar UMETA(DisplayName = "PlayerHotbar"),
	EContainerType_PlayerStorage UMETA(DisplayName = "PlayerStorage"),
	EContainerType_PlayerArmor UMETA(DisplayName = "PlayerArmor"),
};

UENUM(BlueprintType)
enum class EEquipableState : uint8
{
	EEquipableState_Hatchet UMETA(DisplayName = "Hatchet"),
	EEquipableState_Bow UMETA(DisplayName = "Bow"),
	EEquipableState_Rock UMETA(DisplayName = "Rock"),
	EEquipableState_Rifle UMETA(DisplayName = "Rifle"),
	EEquipableState_RocketLauncher UMETA(DisplayName = "RocketLauncher"),
	EEquipableState_Default UMETA(DisplayName = "Default"),
	EEquipableState_Spear UMETA(DisplayName = "Spear"),
	EEquipableState_Swimming UMETA(DisplayName = "Swimming"),
};

UENUM(BlueprintType)
enum class EHarvestingToolType : uint8
{
	EHarvestingToolType_Pickaxe UMETA(DisplayName = "Pickaxe"),
	EHarvestingToolType_Hatchet UMETA(DisplayName = "Hatchet"),
};

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	EItemRarity_Common UMETA(DisplayName = "Common"),
	EItemRarity_Uncommon UMETA(DisplayName = "Uncommon"),
	EItemRarity_Rare UMETA(DisplayName = "Rare"),
	EItemRarity_Epic UMETA(DisplayName = "Epic"),
	EItemRarity_Legendary UMETA(DisplayName = "Legendary"),
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	EItemType_Resource UMETA(DisplayName = "Resource"),
	EItemType_Equipable UMETA(DisplayName = "Equipable"),
	EItemType_Armor UMETA(DisplayName = "Armor"),
	EItemType_Buildable UMETA(DisplayName = "Buildable"),
	EItemType_Consumable UMETA(DisplayName = "Consumable"),
};

UENUM(BlueprintType)
enum class EToolTier : uint8
{
	EToolTier_Stone UMETA(DisplayName = "Stone"),
	EToolTier_Iron UMETA(DisplayName = "Iron"),
};

UENUM(BlueprintType)
enum class ECraftingType : uint8
{
	PlayerInventory,
	CookingPot,
	CraftingBench,
	Forge,
	AdvancedWorkbench,
	StorageBox,
};

UENUM(BlueprintType)
enum class EItemCategory : uint8
{
	Misc,
	Tools,
	Armor,
	Structures,
};

UENUM(BlueprintType)
enum class EPlayerStats : uint8
{
	Health,
	Food,
	Water,
	Stam,
};

USTRUCT(BlueprintType)
struct FResource : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	FResource()
	{
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 ResourceID;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Quantity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EHarvestingToolType PreferredToolType;
};

USTRUCT(BlueprintType)
struct FItemInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	FItemInfo()
	{
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 ItemID;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 ItemQuanity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTexture2D* ItemIcon;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText ItemName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText ItemDesc;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 ItemDamage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EItemRarity ItemRarity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 ItemCurrHP;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 ItemMaxHP;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool IsStackable;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 StackSize;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EItemType ItemType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AItemMaster> ItemClassRef;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EArmorType ArmorType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool UseAmmo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 CurrAmmo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxAmmo;
};

USTRUCT(BlueprintType)
struct FEquipableInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "EquipableInfo")
	FName SocketName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "EquipableInfo")
	EEquipableState AnimationState;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "EquipableInfo")
	TSubclassOf<AEquipableMaster> FirstPersonEquipClass;
};

USTRUCT(BlueprintType)
struct FLargeItem : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	FLargeItem()
	{
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FResource> Resource;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ADestructableHarvestable> Class;
};

USTRUCT(BlueprintType)
struct FItem : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 ItemID;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 ItemQuantity;
};

USTRUCT(BlueprintType)
struct FItemRecipe : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText ItemName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 ItemID;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText ItemDescription;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 RequiredLevel;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTexture2D* ItemIcon;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FItem> RequiredItems;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EItemCategory ItemCategory;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 ItemXP;
};

USTRUCT(BlueprintType)
struct FPlayerStats
{
	GENERATED_USTRUCT_BODY()

	FPlayerStats() {}
	
	FPlayerStats(float CurrHealth, float MaxHealth, float CurrFood, float MaxFood, float CurrWater, float MaxWater,
	             float CurrStam, float MaxStam, int32 CurrLevel, int32 CurrXP,
	             int32 SkillPoints) : CurrHealth(CurrHealth), MaxHealth(MaxHealth), CurrFood(CurrFood),
	                                  MaxFood(MaxFood), CurrWater(CurrWater), MaxWater(MaxWater), CurrStam(CurrStam),
	                                  MaxStam(MaxStam), CurrLevel(CurrLevel), CurrXP(CurrXP), SkillPoints(SkillPoints)
	{
	}

	float CurrHealth;
	float MaxHealth;
	float CurrFood;
	float MaxFood;
	float CurrWater;
	float MaxWater;
	float CurrStam;
	float MaxStam;
	int32 CurrLevel;
	int32 CurrXP;
	int32 SkillPoints;
};

USTRUCT(BlueprintType)
struct FConsumablesStats : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EPlayerStats StatToModify;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool InstantEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RestoreAmount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool OverTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float OverTimeAmount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float OverTimeDuration; 
};

USTRUCT(BlueprintType)
struct FConsumablesInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Name;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FConsumablesStats> StatsToUpdate; 
};
