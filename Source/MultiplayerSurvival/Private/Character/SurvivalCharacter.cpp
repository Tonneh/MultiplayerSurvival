// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SurvivalCharacter.h"

#include "IDetailTreeNode.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DataTable.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Inventory/Hotbar.h"
#include "Inventory/PlayerInventory.h"
#include "Items/FirstPersonEquipable.h"
#include "Items/ThirdPersonEquipable.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "MultiplayerSurvival/Types.h"
#include "Net/UnrealNetwork.h"
#include "PlayerController/SurvivalPlayerController.h"
#include "Sound/SoundCue.h"

ASurvivalCharacter::ASurvivalCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
	FirstPersonCamera->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f));
	FirstPersonCamera->bUsePawnControlRotation = true;

	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	FirstPersonMesh->SetupAttachment(FirstPersonCamera);
	FirstPersonMesh->bOnlyOwnerSee = true;
	FirstPersonMesh->bCastDynamicShadow = false;
	FirstPersonMesh->CastShadow = false;
	FirstPersonMesh->SetRelativeLocation(FVector(6.f, 0.f, -158.f));
	FirstPersonMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	ThirdPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ThirdPersonMesh"));
	ThirdPersonMesh->SetupAttachment(GetCapsuleComponent());
	ThirdPersonMesh->SetRelativeLocation(FVector(6.f, 0.f, -158.f));
	ThirdPersonMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	ThirdPersonMesh->bOwnerNoSee = true;

	SurvivalCharacterArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("SurvivalCharacterArrow"));
	SurvivalCharacterArrow->SetupAttachment(GetCapsuleComponent());


	Inventory = CreateDefaultSubobject<UPlayerInventory>(TEXT("Inventory"));
	Hotbar = CreateDefaultSubobject<UHotbar>(TEXT("Hotbar"));

	bReplicates = true;
}

void ASurvivalCharacter::BeginPlay()
{
	Super::BeginPlay();

	SurvivalPlayerController = SurvivalPlayerController == nullptr
		                           ? Cast<ASurvivalPlayerController>(GetController())
		                           : SurvivalPlayerController;
	EquipableState = EEquipableState::EEquipableState_Default;

	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ASurvivalCharacter::ReceiveDamage);
		ServerDecreaseStatsOverTime();
	}

	FPlayerStats SetPlayerStats = {
		CurrHealth, MaxHealth, CurrFood, MaxFood, CurrWater, MaxWater, CurrStam, MaxStam, CurrLevel, CurrXP, SkillPoints
	};

	PlayerStats = SetPlayerStats;
}

void ASurvivalCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASurvivalCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASurvivalCharacter::Jump);
	PlayerInputComponent->BindAction("Open Inventory", IE_Pressed, this, &ASurvivalCharacter::OpenInventory);
	PlayerInputComponent->BindAction("Left Click", IE_Pressed, this, &ASurvivalCharacter::ServerLeftMouseClicked);
	PlayerInputComponent->BindAction("E", IE_Pressed, this, &ASurvivalCharacter::ServerInteract);
	PlayerInputComponent->BindAction("Left Shift", IE_Pressed, this, &ASurvivalCharacter::Sprint);
	PlayerInputComponent->BindAction("Left Shift", IE_Released, this, &ASurvivalCharacter::StopSprint);

	PlayerInputComponent->BindAxis("Forward", this, &ASurvivalCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Right", this, &ASurvivalCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn Right", this, &ASurvivalCharacter::Turn);
	PlayerInputComponent->BindAxis("Look Up", this, &ASurvivalCharacter::LookUp);
}

void ASurvivalCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ASurvivalCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ASurvivalCharacter::ServerHarvestItem_Implementation(FResource Resource)
{
	HarvestItem(Resource);
}

void ASurvivalCharacter::Sprint()
{
	if (bHaveStam)
	{
		GetCharacterMovement()->MaxWalkSpeed = 800.f;
		ServerSprint();
	}
}

void ASurvivalCharacter::StopSprint()
{
	if (bHaveStam)
	{
		GetCharacterMovement()->MaxWalkSpeed = 600.f;
		ServerStopSprint();
	}
}

void ASurvivalCharacter::ServerStopSprint_Implementation()
{
	if (bHaveStam)
	{
		GetCharacterMovement()->MaxWalkSpeed = 600.f;
		bIsSprinting = false;
	}
}

void ASurvivalCharacter::ServerSprint_Implementation()
{
	if (bHaveStam)
	{
		GetCharacterMovement()->MaxWalkSpeed = 800.f;
		bIsSprinting = true;
	}
}

void ASurvivalCharacter::MoveForward(float Value)
{
	if (bInventoryOpened)
	{
		return;
	}
	AddMovementInput(GetActorForwardVector(), Value);
	if (GetCharacterMovement()->Velocity.Length() > 100.f)
	{
		ServerDrainStam();
	}
	else
	{
		ServerRegenStam();
	}
}

void ASurvivalCharacter::MoveRight(float Value)
{
	if (bInventoryOpened)
	{
		return;
	}
	AddMovementInput(GetActorRightVector(), Value);
}

void ASurvivalCharacter::Turn(float Value)
{
	if (bInventoryOpened)
	{
		return;
	}
	AddControllerYawInput(Value);
}

void ASurvivalCharacter::LookUp(float Value)
{
	if (bInventoryOpened)
	{
		return;
	}
	AddControllerPitchInput(Value);
	// to set where arrow is for melee, temporarily will prob do line trace later
	SurvivalCharacterArrow->SetRelativeLocation(FVector(SurvivalCharacterArrow->GetRelativeLocation().X,
	                                                    SurvivalCharacterArrow->GetRelativeLocation().Y,
	                                                    FMath::Clamp(
		                                                    FirstPersonCamera->GetRelativeRotation().Pitch + 50.4f,
		                                                    -60.f, 95.f)));
}

void ASurvivalCharacter::Jump()
{
	if (bInventoryOpened)
	{
		return;
	}
	Super::Jump();
}

void ASurvivalCharacter::OpenInventory()
{
	SurvivalPlayerController = SurvivalPlayerController == nullptr
		                           ? Cast<ASurvivalPlayerController>(GetController())
		                           : SurvivalPlayerController;
	bInventoryOpened = !bInventoryOpened;
	if (SurvivalPlayerController)
	{
		SurvivalPlayerController->ClientOpenInventory();
	}
}

void ASurvivalCharacter::UseHotbar(const int32 Index)
{
	const bool b = HotbarIndex == Index;
	if (EItemType IndexItemType; Hotbar->CheckHotbar(Index, IndexItemType))
	{
		switch (IndexItemType)
		{
			case EItemType::EItemType_Equipable:
				if (ThirdPersonEquippedItem && b)
				{
					if (HasAuthority())
					{
						ServerUnequipCurrentItem(Index);
					}
				}
				else if (ThirdPersonEquippedItem && !b && bCanSwap)
				{
					if (HasAuthority())
					{
						ServerUnequipCurrentItem(Index);
						HotbarIndex = Index;
						const FItemInfo Item = Hotbar->GetItemAtIndex(HotbarIndex);
						ServerSpawnEquipableThirdPerson(Item.ItemClassRef, Item, HotbarIndex);
					}
				}
				else if (!ThirdPersonEquippedItem)
				{
					if (HasAuthority())
					{
						HotbarIndex = Index;
						const FItemInfo Item = Hotbar->GetItemAtIndex(HotbarIndex);
						ServerSpawnEquipableThirdPerson(Item.ItemClassRef, Item, HotbarIndex);
					}
				}
				break;
			case EItemType::EItemType_Consumable:
				ConsumeItem(Index, EContainerType::EContainerType_PlayerHotbar);
				break;
			case EItemType::EItemType_Buildable:
				break;
			default:
				break;
		}
	}
}

void ASurvivalCharacter::HarvestItem(FResource Resource)
{
	if (ItemDataTable)
	{
		if (FItemInfo* Item = ItemDataTable->FindRow<FItemInfo>(FName(FString::FromInt(Resource.ResourceID)), ""))
		{
			Item->ItemQuanity = Resource.Quantity;
			if (Inventory && Item->ItemID != 0)
			{
				Inventory->ServerAddItem(*Item);
				SurvivalPlayerController = SurvivalPlayerController == nullptr
					                           ? Cast<ASurvivalPlayerController>(GetController())
					                           : SurvivalPlayerController;
				if (SurvivalPlayerController)
				{
					SurvivalPlayerController->ClientShowHarvestedItemWidget(
						Item->ItemIcon, Item->ItemQuanity, FName(Item->ItemName.ToString()));
				}
			}
		}
	}
}

void ASurvivalCharacter::HarvestGroundItem(AActor* HarvestedActor)
{
	UGameplayStatics::ApplyDamage(HarvestedActor, 15.f, SurvivalPlayerController, this, UDamageType::StaticClass());
}

void ASurvivalCharacter::ResetHarvestGround()
{
	bIsHarvesting = !bIsHarvesting;
}

bool ASurvivalCharacter::CanCraftItem(int32 ItemID, EContainerType ContainerType, ECraftingType TableType)
{
	switch (TableType)
	{
		case ECraftingType::PlayerInventory:
			CraftingRecipesDataTable = LoadObject<UDataTable>(nullptr, UTF8_TO_TCHAR(
				                                                  "/Script/Engine.DataTable'/Game/Blueprints/DataTables/DT_PlayerItemRecipes.DT_PlayerItemRecipes'"));
			if (const FItemRecipe* ItemRecipe = CraftingRecipesDataTable->FindRow<FItemRecipe>(
				FName(FString::FromInt(ItemID)), "", true))
			{
				switch (ContainerType)
				{
					case EContainerType::EContainerType_PlayerInventory:
						return Inventory->ContainsItems(ItemRecipe->RequiredItems);
					default:
						break;
				}
				break;
			}
			break;
		default:
			break;
	}
	return false;
}

bool ASurvivalCharacter::CraftItem(int32 ItemID, EContainerType ContainerType, ECraftingType TableType)
{
	if (bIsCrafting)
	{
		return false;
	}
	switch (TableType)
	{
		case ECraftingType::PlayerInventory:
			CraftingRecipesDataTable = LoadObject<UDataTable>(nullptr, UTF8_TO_TCHAR(
				                                                  "/Script/Engine.DataTable'/Game/Blueprints/DataTables/DT_PlayerItemRecipes.DT_PlayerItemRecipes'"));
			if (CraftingRecipesDataTable)
			{
				if (const FItemRecipe* ItemRecipe = CraftingRecipesDataTable->FindRow<FItemRecipe>(
					FName(FString::FromInt(ItemID)), ""))
				{
					switch (ContainerType)
					{
						case EContainerType::EContainerType_PlayerInventory:
							if (Inventory->ContainsItems(ItemRecipe->RequiredItems))
							{
								Inventory->RemoveItems(ItemRecipe->RequiredItems);
								return true;
							}
							return false;
						default:
							break;
					}
				}
			}
			break;
		default:
			break;
	}
	return false;
}

void ASurvivalCharacter::AddCraftedItem(FName ItemIDToAdd, EContainerType ContainerType)
{
	ItemDataTable = LoadObject<UDataTable>(
		nullptr, UTF8_TO_TCHAR("/Script/Engine.DataTable'/Game/Blueprints/DataTables/DT_Items.DT_Items'"));
	if (const FItemInfo* Item = ItemDataTable->FindRow<FItemInfo>(ItemIDToAdd, ""))
	{
		switch (ContainerType)
		{
			case EContainerType::EContainerType_PlayerInventory:
				Inventory->ServerAddItem(*Item);
				bIsCrafting = false;
				break;
			default:
				break;
		}
	}
}

void ASurvivalCharacter::ServerCraftItemOnServer_Implementation(int32 ItemID, EContainerType ContainerType,
                                                                ECraftingType TableType)
{
	if (!bIsCrafting && CraftItem(ItemID, ContainerType, TableType))
	{
		bIsCrafting = true;
		SurvivalPlayerController->ClientShowProgressBar();
		FTimerDelegate CraftTimer;
		CraftTimer.BindUFunction(this, FName("AddCraftedItem"), FName(*FString::FromInt(ItemID)), ContainerType);
		GetWorldTimerManager().SetTimer(CraftingTimerHandle, CraftTimer, CraftingTime, false);
	}
}

void ASurvivalCharacter::ServerCheckIfCanCraftItem_Implementation(int32 ItemID, EContainerType ContainerType,
                                                                  ECraftingType TableType)
{
	bool CanCraft = CanCraftItem(ItemID, ContainerType, TableType);
	if (SurvivalPlayerController)
	{
		SurvivalPlayerController->ClientUpdateCraftStatus(CanCraft);
	}
}

void ASurvivalCharacter::MulticastBushFX_Implementation()
{
	if (HarvestBushFX && HarvestBushSound)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, HarvestBushFX, GetActorLocation());
		UGameplayStatics::PlaySoundAtLocation(this, HarvestBushSound, GetActorLocation());
	}
}

void ASurvivalCharacter::MulticastHarvestGroundAnim_Implementation()
{
	float Length = ThirdPersonMesh->GetAnimInstance()->Montage_Play(ThirdPersonPickupMontage);
	Length -= 1.f;
	GetWorldTimerManager().SetTimer(AnimTimerHandle, this, &ASurvivalCharacter::ResetHarvestGround, Length);
}

void ASurvivalCharacter::ServerOverlapGroundItems_Implementation()
{
	if (!bIsHarvesting && !ThirdPersonEquippedItem)
	{
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
		TArray<AActor*> IgnoreActors = {this};
		TArray<AActor*> OutActors;
		bool bHit = UKismetSystemLibrary::SphereOverlapActors(this, GetActorLocation(), 70.f, ObjectTypes,
		                                                      AActor::StaticClass(),
		                                                      IgnoreActors, OutActors);
		DrawDebugSphere(GetWorld(), GetActorLocation(), 70.f, 12, FColor::Red, false, 2, 0, 5);
		if (bHit)
		{
			for (AActor* Actor : OutActors)
			{
				HarvestGroundItem(Actor);
				if (ThirdPersonPickupMontage && FirstPersonPickupMontage)
				{
					bIsHarvesting = true;
					MulticastHarvestGroundAnim();
					ClientPlayFirstPersonMontage(FirstPersonPickupMontage);
					MulticastBushFX();
				}
			}
		}
	}
}

void ASurvivalCharacter::ServerInteract_Implementation()
{
	ServerOverlapGroundItems();
}

void ASurvivalCharacter::ServerUnequipCurrentItem_Implementation(int32 Index)
{
	if (ThirdPersonEquippedItem)
	{
		MulticastUnequipWeaponThirdPerson();
		ClientUnequipWeaponFirstPerson();
		ThirdPersonEquippedItem->Destroy();
		ThirdPersonEquippedItem = nullptr;
	}
}

void ASurvivalCharacter::MulticastPlayThirdPersonMontage_Implementation(UAnimMontage* Animation)
{
	float AnimLength = ThirdPersonMesh->GetAnimInstance()->Montage_Play(Animation);
	AnimLength -= 0.50f;
	GetWorldTimerManager().SetTimer(AnimTimerHandle, ThirdPersonEquippedItem, &AThirdPersonEquipable::EndAnim,
	                                AnimLength);
}

void ASurvivalCharacter::ClientPlayFirstPersonMontage_Implementation(UAnimMontage* Animation)
{
	FirstPersonMesh->GetAnimInstance()->Montage_Play(Animation);
}

void ASurvivalCharacter::ServerLeftMouseClicked_Implementation()
{
	if (bInventoryOpened)
	{
		return;
	}
	if (ThirdPersonEquippedItem)
	{
		bCanSwap = false;
		ThirdPersonEquippedItem->UseEquippable();
	}
}

void ASurvivalCharacter::MulticastUnequipWeaponThirdPerson_Implementation()
{
	if (ThirdPersonEquippedItem && ThirdPersonEquippedItem->GetUnequipAnim())
	{
		ThirdPersonMesh->GetAnimInstance()->Montage_Play(
			ThirdPersonEquippedItem->GetUnequipAnim());
	}
	EquipableState = EEquipableState::EEquipableState_Default;
}

void ASurvivalCharacter::ClientUnequipWeaponFirstPerson_Implementation()
{
	if (FirstPersonEquipable && FirstPersonEquipable->GetUnequipAnim())
	{
		FirstPersonMesh->GetAnimInstance()->Montage_Play(FirstPersonEquipable->GetUnequipAnim());
		FirstPersonEquipable->Destroy();
		FirstPersonEquipable = nullptr;
	}
}

void ASurvivalCharacter::ClientSpawnEquipableFirstPerson_Implementation(TSubclassOf<AActor> EquipableClass,
                                                                        const FName SocketName)
{
	FirstPersonEquipable = Cast<AFirstPersonEquipable>(GetWorld()->SpawnActor(EquipableClass));
	if (FirstPersonEquipable)
	{
		FirstPersonEquipable->AttachToComponent(FirstPersonMesh, FAttachmentTransformRules::SnapToTargetIncludingScale,
		                                        SocketName);
		if (FirstPersonEquipable->GetEquipAnim())
		{
			FirstPersonMesh->GetAnimInstance()->Montage_Play(FirstPersonEquipable->GetEquipAnim(), 3.f);
			FirstPersonEquipable->SetOwner(this);
		}
	}
}

void ASurvivalCharacter::MulticastWeaponEquip_Implementation(AActor* Target, const FName SocketName,
                                                             const EEquipableState EquippedState)
{
	EquipableState = EquippedState;
	if (ThirdPersonEquippedItem && ThirdPersonEquippedItem->GetEquipAnim())
	{
		ThirdPersonMesh->GetAnimInstance()->Montage_Play(ThirdPersonEquippedItem->GetEquipAnim());
		Target->AttachToComponent(ThirdPersonMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
	}
}

void ASurvivalCharacter::ServerSpawnEquipableThirdPerson_Implementation(TSubclassOf<AActor> EquipableClass,
                                                                        const FItemInfo ItemInfo, const int32 ItemIndex)
{
	if (EquipableClass)
	{
		EquippedIndex = ItemIndex;
		ThirdPersonEquippedItem = Cast<AThirdPersonEquipable>(GetWorld()->SpawnActor(EquipableClass));
		if (ThirdPersonEquippedItem && HasAuthority() && !IsLocallyControlled())
		{
			ThirdPersonEquippedItem->SetOwner(this);
			MulticastWeaponEquip(ThirdPersonEquippedItem,
			                     ThirdPersonEquippedItem->GetEquipableInfo().SocketName,
			                     ThirdPersonEquippedItem->GetEquipableInfo().AnimationState);
			ClientSpawnEquipableFirstPerson(ThirdPersonEquippedItem->GetEquipableInfo().FirstPersonEquipClass,
			                                ThirdPersonEquippedItem->GetEquipableInfo().SocketName);
		}
	}
}

void ASurvivalCharacter::ServerHotbarInput_Implementation(const int32 Index)
{
	UseHotbar(Index);
}

void ASurvivalCharacter::ServerOnSlotDrop_Implementation(EContainerType TargetContainer, EContainerType SourceContainer,
                                                         int32 TargetIndex, int32 SourceIndex, EArmorType ArmorType)
{
	if (SourceContainer == EContainerType::EContainerType_PlayerInventory)
	{
		if (TargetContainer == EContainerType::EContainerType_PlayerInventory)
		{
			Inventory->ServerOnSlotDrop(Inventory, SourceIndex, TargetIndex);
		}
		else if (TargetContainer == EContainerType::EContainerType_PlayerHotbar)
		{
			Inventory->ServerOnSlotDrop(Hotbar, SourceIndex, TargetIndex);
		}
	}
	else if (SourceContainer == EContainerType::EContainerType_PlayerHotbar)
	{
		if (TargetContainer == EContainerType::EContainerType_PlayerInventory)
		{
			Hotbar->ServerOnSlotDrop(Inventory, SourceIndex, TargetIndex);
		}
		else if (TargetContainer == EContainerType::EContainerType_PlayerHotbar)
		{
			Hotbar->ServerOnSlotDrop(Hotbar, SourceIndex, TargetIndex);
		}
	}
}


void ASurvivalCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                                       AController* InstigatorController, AActor* DamageCauser)
{
	if (bIsDead)
	{
		return;
	}
	float LocalHealth = PlayerStats.CurrHealth - Damage;
	PlayerStats.CurrHealth = LocalHealth;
	if (LocalHealth <= 0.f)
	{
		bIsDead = true;
	}
	SurvivalPlayerController = SurvivalPlayerController == nullptr
		                           ? Cast<ASurvivalPlayerController>(GetController())
		                           : SurvivalPlayerController;
	if (SurvivalPlayerController)
	{
		SurvivalPlayerController->ClientUpdateStatBar(EPlayerStats::Health, LocalHealth, PlayerStats.MaxHealth);
	}
}

void ASurvivalCharacter::FoodWaterDrain()
{
	SurvivalPlayerController = SurvivalPlayerController == nullptr
		                           ? Cast<ASurvivalPlayerController>(GetController())
		                           : SurvivalPlayerController;
	if (bIsDead)
	{
		GetWorldTimerManager().ClearTimer(StatDrainTimer);
		return;
	}
	float Food = DecreaseFloat(PlayerStats.CurrFood, 0.02f, PlayerStats.MaxFood);
	float Water = DecreaseFloat(PlayerStats.CurrWater, 0.02f, PlayerStats.MaxWater);
	if (Food == 0.0f)
	{
		bIsStarving = true;
		if (!GetWorld()->GetTimerManager().IsTimerActive(HealthDrainTimer))
		{
			ServerDecreaseHealthOverTime();
		}
		SurvivalPlayerController->ClientShowOrHideStarving(true);
	}
	if (Water == 0.0f)
	{
		bIsDehydrated = true;
		if (!GetWorld()->GetTimerManager().IsTimerActive(HealthDrainTimer))
		{
			// Dehydrating so start the health drain
			ServerDecreaseHealthOverTime();
		}
		SurvivalPlayerController->ClientShowOrHideDehydration(true);
	}
	PlayerStats.CurrFood = Food;
	PlayerStats.CurrWater = Water;

	if (SurvivalPlayerController)
	{
		SurvivalPlayerController->ClientUpdateStatBar(EPlayerStats::Food, PlayerStats.CurrFood, PlayerStats.MaxFood);
		SurvivalPlayerController->ClientUpdateStatBar(EPlayerStats::Water, PlayerStats.CurrWater, PlayerStats.MaxWater);
	}
}

float ASurvivalCharacter::DecreaseFloat(float FloatToDecrease, float Percentage, float Max)
{
	return FMath::Clamp(FloatToDecrease - (Max * Percentage), 0.f, Max);
}


void ASurvivalCharacter::ServerDecreaseStatsOverTime_Implementation()
{
	GetWorldTimerManager().SetTimer(StatDrainTimer, this, &ASurvivalCharacter::FoodWaterDrain, FoodWaterDrainSpeed,
	                                true);
}

void ASurvivalCharacter::DecreaseHealth()
{
	if (bIsDead)
	{
		GetWorldTimerManager().ClearTimer(HealthDrainTimer);
	}
	if (bIsStarving && bIsDehydrated)
	{
		// Starving and Dehydrated so drain more health
		UGameplayStatics::ApplyDamage(this, 0.4f, GetController(), this, UDamageType::StaticClass());
	}
	if (bIsStarving || bIsDehydrated)
	{
		UGameplayStatics::ApplyDamage(this, 0.2f, GetController(), this, UDamageType::StaticClass());
	}
}

void ASurvivalCharacter::ServerDecreaseHealthOverTime_Implementation()
{
	GetWorldTimerManager().SetTimer(HealthDrainTimer, this, &ASurvivalCharacter::DecreaseHealth, 0.3f, true);
}

void ASurvivalCharacter::StartDrainTimer()
{
	if (bShouldDrainStam)
	{
		UE_LOG(LogTemp, Warning, TEXT("Drain"));
		DrainStam();
	}
	if (bIsSprinting)
	{
		GetWorldTimerManager().SetTimer(StamDrainTimer, this, &ASurvivalCharacter::StartDrainTimer, 0.1f);
	}
}

void ASurvivalCharacter::DrainStam()
{
	if (PlayerStats.CurrStam == 0.f)
	{
		return;
	}
	float LocalStam = FMath::Clamp(PlayerStats.CurrStam - 0.8f, 0.f, PlayerStats.MaxStam);
	PlayerStats.CurrStam = LocalStam;
	SurvivalPlayerController = SurvivalPlayerController == nullptr
		                           ? Cast<ASurvivalPlayerController>(GetController())
		                           : SurvivalPlayerController;
	if (SurvivalPlayerController)
	{
		SurvivalPlayerController->ClientUpdateStatBar(EPlayerStats::Stam, LocalStam, PlayerStats.MaxStam);
	}
}

void ASurvivalCharacter::ServerDrainStam_Implementation()
{
	if (bDoOnceStam && bIsSprinting)
	{
		bDoOnceStam = false;
		bShouldDrainStam = true;
		StartDrainTimer();
	}
}

void ASurvivalCharacter::StartStamRegenTimer()
{
	if (!bShouldDrainStam)
	{
		RegenStam();
	}
	GetWorldTimerManager().SetTimer(StamRegenTimer, this, &ASurvivalCharacter::StartStamRegenTimer, 0.3f);
}

void ASurvivalCharacter::RegenStam()
{
	if (PlayerStats.CurrStam == PlayerStats.MaxStam)
	{
		return;
	}
	const float LocalStam = FMath::Clamp(PlayerStats.CurrStam + 0.02f, 0.f, PlayerStats.MaxStam);
	PlayerStats.CurrStam = LocalStam;
	SurvivalPlayerController = SurvivalPlayerController == nullptr
		                           ? Cast<ASurvivalPlayerController>(GetController())
		                           : SurvivalPlayerController;
	if (SurvivalPlayerController)
	{
		SurvivalPlayerController->ClientUpdateStatBar(EPlayerStats::Stam, LocalStam, PlayerStats.MaxStam);
	}
}

void ASurvivalCharacter::ServerRegenStam_Implementation()
{
	bShouldDrainStam = false;
	StartStamRegenTimer();
	bDoOnceStam = true;
}

void ASurvivalCharacter::ConsumeItem(int32 ItemIndex, EContainerType ContainerType)
{
	FItemInfo Item;
	switch (ContainerType)
	{
		case EContainerType::EContainerType_PlayerInventory:
			Item = Inventory->GetItemAtIndex(ItemIndex);
			Inventory->RemoveQuantity(ItemIndex, 1);
			break;
		case EContainerType::EContainerType_PlayerHotbar:
			Item = Hotbar->GetItemAtIndex(ItemIndex);
			Hotbar->RemoveQuantity(ItemIndex, 1);
			break;
		default:
			break;
	}

	UDataTable* ConsumablesDataTable = LoadObject<UDataTable>(nullptr, UTF8_TO_TCHAR(
		                                                          "/Script/Engine.DataTable'/Game/Blueprints/DataTables/DT_Consumables.DT_Consumables'"));
	ConsumablesDataTable->AddToRoot();
	if (ConsumablesDataTable)
	{
		FConsumablesInfo* Consumable = ConsumablesDataTable->FindRow<FConsumablesInfo>(
			FName(FString::FromInt(Item.ItemID)), "");
		if (Consumable)
		{
			for (const FConsumablesStats StatToUpdate : Consumable->StatsToUpdate)
			{
				if (StatToUpdate.InstantEffect)
				{
					InstantUpdateStat(StatToUpdate.StatToModify, StatToUpdate.RestoreAmount);
				}
			}
		}
	}
}

void ASurvivalCharacter::InstantUpdateStat(EPlayerStats StatToChange, float Amount)
{
	switch (StatToChange)
	{
		case EPlayerStats::Health:
			PlayerStats.CurrHealth += FMath::Clamp(Amount, 0.f, PlayerStats.MaxHealth);
			if (SurvivalPlayerController)
			{
				SurvivalPlayerController->ClientUpdateStatBar(EPlayerStats::Health, PlayerStats.CurrHealth,
				                                              PlayerStats.MaxHealth);
			}
			break;
		case EPlayerStats::Food:
			PlayerStats.CurrFood += FMath::Clamp(Amount, 0.f, PlayerStats.MaxFood);
			if (bIsStarving && SurvivalPlayerController)
			{
				bIsStarving = false;
				SurvivalPlayerController->ShowOrHideStarving(false);
				SurvivalPlayerController->ClientUpdateStatBar(EPlayerStats::Food, PlayerStats.CurrFood,
				                                              PlayerStats.MaxFood);
			}
			break;
		case EPlayerStats::Water:
			PlayerStats.CurrWater += FMath::Clamp(Amount, 0.f, PlayerStats.MaxWater);
			if (bIsDehydrated && SurvivalPlayerController)
			{
				bIsDehydrated = false;
				SurvivalPlayerController->ShowOrHideDehydration(false);
				SurvivalPlayerController->ClientUpdateStatBar(EPlayerStats::Water, PlayerStats.CurrWater,
				                                              PlayerStats.MaxWater);
			}
			break;
		case EPlayerStats::Stam:
			PlayerStats.CurrStam += FMath::Clamp(Amount, 0.f, PlayerStats.MaxStam);
			if (SurvivalPlayerController)
			{
				SurvivalPlayerController->ClientUpdateStatBar(EPlayerStats::Stam, PlayerStats.CurrStam,
				                                              PlayerStats.MaxStam);
			}
			break;
		default:
			break;
	}
}
