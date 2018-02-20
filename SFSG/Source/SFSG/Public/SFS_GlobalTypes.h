// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SFS_GlobalTypes.generated.h"


UENUM(BlueprintType)
enum class EInteractionEvent : uint8
{
	PickupItem,
	AccessContainer,
	UseButton,
};

UENUM(BlueprintType)
enum class EItemCategory : uint8
{
	Material,
	Usable,
	Placeable,
	Complex,
};

UENUM(BlueprintType)
enum class EItemPropertyType : uint8
{
	Durability,
	EquipSlot,
};

USTRUCT(BlueprintType)
struct FItemProperties
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties")
	EItemPropertyType Property;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties")
	int32 PropertyValue;

};

USTRUCT(BlueprintType)
struct FInventoryStruct
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	class USFSItemBase* ItemInstance;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	int32 Quantity;
};

UCLASS()
class SFSG_API USFS_GlobalTypes : public UObject
{
	GENERATED_BODY()
	
};
