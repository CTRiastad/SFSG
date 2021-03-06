// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SFS_GlobalTypes.h"
#include "SFSItemBase.generated.h"

USTRUCT(BlueprintType)
struct FInvData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	UTexture2D* Icon;
};

UCLASS(Blueprintable)
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

	/** Does this item have a distinct appearance in the world */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties")
	bool bIsWorldItem;

	/** If it has a distinct appearance; set with this */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (EditCondition = bIsWorldItem))
	class UStaticMesh* WorldMesh;

	/** Does this item stack */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties")
	bool bIsStackable;
	
	/** Maximum stack size */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (Editcondition = bIsStackable))
	int32 StackCapacity = 1;

	/** Type of item */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties")
	EItemCategory ItemCategory;
	
	/** Allows the addition of extra item properties via FItemProperties */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties")
	bool bHasComplexProperties;

	/** Allow dynamic addition of extra item properties */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (Editcondition = bHasComplexProperties))
	TArray<FItemProperties> ItemProperties;


public:

	/** Flag for initialization state of complex item properties */
	bool bIsComplexInitialized;

	/** Populates the item properties for item instance */
	void InitializeComplexProperties();

	int32 GetMaxStackSize();

	int32 GetPropertyValue(EItemPropertyType ItemProperty);

	UFUNCTION(BlueprintImplementableEvent)
	void OnUsed();

	UStaticMesh* GetWorldMesh();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	FInvData GetInvData();

	virtual bool IsNameStableForNetworking() const override;

	virtual bool IsSupportedForNetworking() const override;
};
