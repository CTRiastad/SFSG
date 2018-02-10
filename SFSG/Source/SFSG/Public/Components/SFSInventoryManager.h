// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SFS_GlobalTypes.h"
#include "SFSInventoryManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryUpdateSignature, int32, UpdatedSlot);

UENUM(BlueprintType)
enum class EInventoryAction : uint8
{
	RemoveItem,
	MoveItem,
	SplitStack,
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

	/** Sets the size of the array on begin play and sets each element to a default value **/
	void InitializeArray();

	/** Sets the inventory "slot" to a default/invalid value **/
	void ClearElementAtIndex(int32 IndexRef);

	/** Creates item instance to be added to inventory **/
	class USFSItemBase* GenerateItemObject(TSubclassOf<USFSItemBase> ItemClass);

	/** Attempts to add item to inventory using a class, or existing object **/
	bool AddItemToInventory(USFSItemBase* ItemToAdd, int32 Quantity = 1);
	bool AddItemToInventory(TSubclassOf<USFSItemBase> ItemClassToAdd, int32 QuantityToAdd = 1);

	/** Helper functions that find if there is a valid spot in the inventory **/
	bool FindEmptySlot(int32& IndexRef);
	bool FindValidStack(TSubclassOf<USFSItemBase> ItemClassToAdd, int32& IndexRef, int32 QuantityToAdd = 1);

	/** Decreases the quantity count of a stack, or clears an item from the inventory entirely **/
	void RemoveItemFromInventory(int32 IndexRef, int32 QuantityToRemove);

	/** Moves an item to a different inventory slot internally **/
	void SwapItems(int32 FirstIndexRef, int32 SecondIndexRef);

	/** Splits an amount of an existing item stack into a new item stack **/
	void SplitStack(int32 IndexRef, int32 QuantityToSplit);

	/** Client RPC that manages the client-copy of the inventory **/
	UFUNCTION(Client, Reliable)
	void Client_RemoveItem(int32 IndexRef, int32 QuantityToRemove);

	/** Client RPC that manages the client-copy of the inventory **/
	UFUNCTION(Client, Reliable)
	void Client_AddItem(int32 IndexRef, int32 QuantityToAdd = 1, TSubclassOf<USFSItemBase> ItemClass = nullptr);

	/** Client RPC that manages the client-copy of the inventory **/
	UFUNCTION(Client, Reliable)
	void Client_SwapItem(int32 FirstIndexRef, int32 SecondIndexRef);

	/** Server-authoritative inventory control function **/
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_PerformInventoryAction(EInventoryAction InventoryAction, int32 FirstIndex, int32 SecondIndex = -1, int32 Quantity = 1, AActor* Container = nullptr);

	/** Server-authoritative item pickup function **/
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AttemptItemPickup(class ASFSWorldItemActor* ItemActor);

public:	

	void RequestInventoryAction(EInventoryAction InventoryAction, int32 FirstIndex, int32 SecondIndex = -1, int32 Quantity = 1, AActor* Container = nullptr);

	void AttemptItemPickup(class ASFSWorldItemActor* ItemActor);

	/** Used to get information for UI **/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	FInventoryStruct& GetInventoryStruct(int32 IndexRef);
	
	/** Event delegate for updating UI **/
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FInventoryUpdateSignature UpdateClientEvent;

};
