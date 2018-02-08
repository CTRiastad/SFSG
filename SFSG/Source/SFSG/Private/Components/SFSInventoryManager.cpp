// Fill out your copyright notice in the Description page of Project Settings.

#include "SFSInventoryManager.h"
#include "SFSItemBase.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
USFSInventoryManager::USFSInventoryManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	InventorySize = 20;

}


// Called when the game starts
void USFSInventoryManager::BeginPlay()
{
	Super::BeginPlay();

	InitializeArray();
	
}

void USFSInventoryManager::InitializeArray()
{
	InvArray.SetNum(InventorySize);
	for (int32 i = 0; i < InvArray.Num(); i++)
	{
		ClearElementAtIndex(i);
	}
}

void USFSInventoryManager::ClearElementAtIndex(int32 IndexRef)
{
	InvArray[IndexRef].ItemInstance = nullptr;

}

class USFSItemBase* USFSInventoryManager::GenerateItemObject(TSubclassOf<USFSItemBase> ItemClass)
{
	return NewObject<USFSItemBase>(this, ItemClass);
}

bool USFSInventoryManager::AddItemToInventory(USFSItemBase* ItemToAdd, int32 QuantityToAdd)
{
	int32 IndexRef;
	if (FindValidStack(ItemToAdd->GetClass(), IndexRef, QuantityToAdd))
	{
		InvArray[IndexRef].ItemInstance = ItemToAdd;
		InvArray[IndexRef].Quantity = QuantityToAdd;
		return true;
	}
	if (FindEmptySlot(IndexRef))
	{
		InvArray[IndexRef].ItemInstance = ItemToAdd;
		InvArray[IndexRef].Quantity = QuantityToAdd;
		
		return true;
	}
	return false;
}

bool USFSInventoryManager::AddItemToInventory(TSubclassOf<USFSItemBase> ItemClassToAdd, int32 QuantityToAdd)
{
	int32 IndexRef;
	if (FindValidStack(ItemClassToAdd, IndexRef, QuantityToAdd))
	{
		InvArray[IndexRef].Quantity += QuantityToAdd;
		return true;
	}
	if (FindEmptySlot(IndexRef))
	{
		InvArray[IndexRef].ItemInstance = GenerateItemObject(ItemClassToAdd);
		InvArray[IndexRef].Quantity = QuantityToAdd;
	}
	return false;
}

bool USFSInventoryManager::FindEmptySlot(int32& IndexRef)
{
	IndexRef = InvArray.IndexOfByPredicate([](const FInventoryStruct InvSlot) { return (!InvSlot.ItemInstance); });
	return (IndexRef != -1);
}

bool USFSInventoryManager::FindValidStack(TSubclassOf<USFSItemBase> ItemClassToAdd, int32& IndexRef, int32 QuantityToAdd)
{
	IndexRef = InvArray.IndexOfByPredicate([&ItemClassToAdd, QuantityToAdd](const FInventoryStruct& InvItem)
	{
		return InvItem.ItemInstance ? (InvItem.ItemInstance->GetClass()->IsChildOf(ItemClassToAdd)) && (InvItem.Quantity + QuantityToAdd < InvItem.ItemInstance->GetMaxStackSize()) : false;
	});
	return (IndexRef != -1);
}

void USFSInventoryManager::RequestInventoryAction(EInventoryAction InventoryAction, int32 FirstIndex, int32 SecondIndex /*= -1*/, int32 Quantity /*= 1*/, AActor* Container /*= nullptr*/)
{
	Server_PerformInventoryAction(InventoryAction, FirstIndex, SecondIndex, Quantity, Container);
}

void USFSInventoryManager::AttemptItemPickup(AActor* ItemActor)
{

}

void USFSInventoryManager::Server_PerformInventoryAction_Implementation(EInventoryAction InventoryAction, int32 FirstIndex, int32 SecondIndex /*= -1*/, int32 Quantity /*= 1*/, AActor* Container /*= nullptr*/)
{

}

bool USFSInventoryManager::Server_PerformInventoryAction_Validate(EInventoryAction InventoryAction, int32 FirstIndex, int32 SecondIndex /*= -1*/, int32 Quantity /*= 1*/, AActor* Container /*= nullptr*/)
{
	return true;
}