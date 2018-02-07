// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SFS_GlobalTypes.h"
#include "SFSItemBase.generated.h"

/**
 * 
 */
UCLASS()
class SFSG_API USFSItemBase : public UObject
{
	GENERATED_BODY()
	
public:

protected:

	/** For internal use */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties")
	FName ItemName;
	
	/** Item name shown to user */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties")
	FText ItemDisplayName;

	/** Inventory Icon*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties")
	class UTexture2D* ItemIcon;

	/** Does this item stack */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties")
	bool bIsStackable;
	
	/** Maximum stack size */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (Editcondition = bIsStackable))
	int32 StackCapacity = 1;

	/** Type of item */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties")
	EItemCategory ItemCategory;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties")
	bool bHasDynamicProperties;

	/** Allow dynamic addition of extra item properties */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (Editcondition = bHasDynamicProperties))
	TArray<FItemProperties> ItemProperties;

public:

	/** Flag for initialization state of complex item properties */
	bool bIsComplexInitialized;

	/** Populates the item properties for item instance */
	void InitializeComplexProperties();

};
