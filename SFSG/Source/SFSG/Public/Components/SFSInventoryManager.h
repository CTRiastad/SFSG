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
	AddItem,
	RemoveItem,
	MoveItem,
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

	void InitializeArray();

	void ClearElementAtIndex(int32 IndexRef);

	class USFSItemBase* GenerateItemObject(TSubclassOf<USFSItemBase> ItemClass);

	bool AddItemToInventory(USFSItemBase* ItemToAdd, int32 Quantity = 1);
	bool AddItemToInventory(TSubclassOf<USFSItemBase> ItemClassToAdd, int32 QuantityToAdd = 1);

	void RemoveItemFromInventory(int32 IndexRef, int32 QuantityToRemove);

	bool FindEmptySlot(int32& IndexRef);

	bool FindValidStack(TSubclassOf<USFSItemBase> ItemClassToAdd, int32& IndexRef, int32 QuantityToAdd = 1);

	UFUNCTION(Client, Reliable)
	void Client_UpdateItem(int32 IndexRef, const FInventoryStruct& ElementValue);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_PerformInventoryAction(EInventoryAction InventoryAction, int32 FirstIndex, int32 SecondIndex = -1, int32 Quantity = 1, AActor* Container = nullptr);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AttemptItemPickup(class ASFSWorldItemActor* ItemActor);

public:	

	void RequestInventoryAction(EInventoryAction InventoryAction, int32 FirstIndex, int32 SecondIndex = -1, int32 Quantity = 1, AActor* Container = nullptr);

	void AttemptItemPickup(class ASFSWorldItemActor* ItemActor);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	FInventoryStruct& GetInventoryStruct(int32 IndexRef);
	
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FInventoryUpdateSignature UpdateClientEvent;

};
