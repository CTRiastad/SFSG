// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SFS_GlobalTypes.h"
#include "SFSInventoryManager.generated.h"

UENUM(BlueprintType)
enum class EInventoryAction : uint8
{
	AddItem,
	RemoveItem,
	MoveItem,
};

USTRUCT()
struct FInventoryStruct
{
	GENERATED_BODY()

	UPROPERTY()
	class USFSItemBase* ItemInstance;

	UPROPERTY()
	int32 Quantity;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SFSG_API USFSInventoryManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USFSInventoryManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	TArray<FInventoryStruct> InvArray;

	int32 InventorySize;

	void InitializeArray();

	void ClearElementAtIndex(int32 IndexRef);

	class USFSItemBase* GenerateItemObject(TSubclassOf<USFSItemBase> ItemClass);

	bool AddItemToInventory(USFSItemBase* ItemToAdd, int32 Quantity = 1);
	bool AddItemToInventory(TSubclassOf<USFSItemBase> ItemClassToAdd, int32 QuantityToAdd = 1);

	void RemoveItemFromInventory(int32 IndexRef, int32 QuantityToRemove);

	bool FindEmptySlot(int32& IndexRef);

	bool FindValidStack(TSubclassOf<USFSItemBase> ItemClassToAdd, int32& IndexRef, int32 QuantityToAdd = 1);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_PerformInventoryAction(EInventoryAction InventoryAction, int32 FirstIndex, int32 SecondIndex = -1, int32 Quantity = 1, AActor* Container = nullptr);

public:	

	void RequestInventoryAction(EInventoryAction InventoryAction, int32 FirstIndex, int32 SecondIndex = -1, int32 Quantity = 1, AActor* Container = nullptr);

	void AttemptItemPickup(AActor* ItemActor);
	
};
