// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SFS_GlobalTypes.h"
#include "SFSInventoryManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryUpdateSignature, int32, UpdatedSlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAccessedContainerSignature, class ASFSContainerWorldActor*, ContainerActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FContainerUpdateSignature, class ASFSContainerWorldActor*, ContainerActor, int32, UpdatedSlot);

UENUM(BlueprintType)
enum class EInventoryAction : uint8
{
	RemoveItem,
	MoveItem,
	SplitStack,
	Use,
	CombineStack,
	ContainerTake,
	ContainerPlace,
};

USTRUCT(BlueprintType)
struct FItemLocation
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Inventory")
	TArray<FInventoryStruct>& ContainingArray;

	UPROPERTY(BlueprintReadWrite, Category = "Inventory")
	int32 IndexRef;

};

USTRUCT(BlueprintType)
struct FInventoryActionData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Inventory")
	EInventoryAction InventoryAction;

	UPROPERTY(BlueprintReadWrite, Category = "Inventory")
	int32 FirstIndexRef;

	UPROPERTY(BlueprintReadWrite, Category = "Inventory")
	int32 SecondIndexRef;

	UPROPERTY(BlueprintReadWrite, Category = "Inventory")
	int32 Quantity = 1;

	UPROPERTY(BlueprintReadWrite, Category = "Inventory")
	class ASFSContainerWorldActor* ContainerActor = nullptr;

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

	UPROPERTY(Replicated)
	class ASFSContainerWorldActor* ActiveContainer;

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
	void RemoveItemFromInventory(int32 IndexRef, int32 QuantityToRemove = 0);

	/** Moves an item to a different inventory slot internally **/
	void SwapItems(int32 FirstIndexRef, int32 SecondIndexRef);
	
	/** Activates or consumes an item to perform a given function **/
	bool UseItem(int32 IndexRef);

	/** Combines two existing stacks of the same item type **/
	void CombineStack(int32 FirstIndexRef, int32 SecondIndexRef);

	/** Splits an amount of an existing item stack into a new item stack **/
	int32 SplitStack(int32 IndexRef, int32 QuantityToSplit);

	/** Server-authoritative item pickup function **/
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AttemptItemPickup(class ASFSWorldItemActor* ItemActor);

	UFUNCTION(Client, Reliable)
	void Client_AttemptItemPickup(class ASFSWorldItemActor* ItemActor);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_PerformInventoryAction(const FInventoryActionData& InventoryAction);

	UFUNCTION(Client, Reliable)
	void Client_PerformInventoryAction(const FInventoryActionData& InventoryAction);

	UFUNCTION(Client, Reliable)
	void Client_AccessContainer(class ASFSContainerWorldActor* ContainerActor);

public:	

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void PerformInventoryAction(const FInventoryActionData& ActionRequest);

	void AttemptItemPickup(class ASFSWorldItemActor* ItemActor);

	void AccessContainer(class ASFSContainerWorldActor* ContainerActor);

	/** Used to get information for UI **/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	FInventoryStruct& GetInventoryStruct(int32 IndexRef);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	FInventoryStruct& GetContainerInventoryStruct(int32 IndexRef);
	
	/** Event delegate for updating local UI **/
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FInventoryUpdateSignature UpdateClientEvent;

	/** Event delegate for accessing containers */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FAccessedContainerSignature AccessedContainerEvent;

	/** Event delegate for updating container UI **/
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FContainerUpdateSignature UpdateContainerEvent;

	FORCEINLINE void ClientUpdate(int32 IndexRef) 
	{
		if (GetOwnerRole() < ROLE_Authority)
		{
			UpdateClientEvent.Broadcast(IndexRef);
		}
	}
};