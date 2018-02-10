// Fill out your copyright notice in the Description page of Project Settings.

#include "SFSInventoryManager.h"
#include "SFSWorldItemActor.h"
#include "SFSItemBase.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
USFSInventoryManager::USFSInventoryManager()
{
	InventorySize = 20;
	SetIsReplicated(true);
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
	InvArray[IndexRef].ItemInstance = 0;

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
		Client_UpdateItem(IndexRef, InvArray[IndexRef]);
		return true;
	}

	if (FindEmptySlot(IndexRef))
	{
		InvArray[IndexRef].ItemInstance = ItemToAdd;
		InvArray[IndexRef].Quantity = QuantityToAdd;
		Client_UpdateItem(IndexRef, InvArray[IndexRef]);
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
		Client_UpdateItem(IndexRef, InvArray[IndexRef]);
		return true;
	}

	if (FindEmptySlot(IndexRef))
	{
		InvArray[IndexRef].ItemInstance = GenerateItemObject(ItemClassToAdd);
		InvArray[IndexRef].Quantity = QuantityToAdd;
		Client_UpdateItem(IndexRef, InvArray[IndexRef]);
		return true;
	}

	return false;
}

void USFSInventoryManager::RemoveItemFromInventory(int32 IndexRef, int32 QuantityToRemove)
{
	if (InvArray[IndexRef].ItemInstance)
	{
		if (InvArray[IndexRef].Quantity - QuantityToRemove > 0)
		{
			InvArray[IndexRef].Quantity -= QuantityToRemove;
		}
		else
		{
			ClearElementAtIndex(IndexRef);
		}
	}
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

void USFSInventoryManager::Server_AttemptItemPickup_Implementation(ASFSWorldItemActor* ItemActor)
{
	AttemptItemPickup(ItemActor);
}

bool USFSInventoryManager::Server_AttemptItemPickup_Validate(ASFSWorldItemActor* ItemActor)
{
	return true;
}

void USFSInventoryManager::Client_UpdateItem_Implementation(int32 IndexRef, const FInventoryStruct& ElementValue)
{
	UE_LOG(LogTemp, Log, TEXT("Client Update"))
	InvArray[IndexRef] = ElementValue;
	UpdateClientEvent.Broadcast(IndexRef);
}

void USFSInventoryManager::RequestInventoryAction(EInventoryAction InventoryAction, int32 FirstIndex, int32 SecondIndex /*= -1*/, int32 Quantity /*= 1*/, AActor* Container /*= nullptr*/)
{
	Server_PerformInventoryAction(InventoryAction, FirstIndex, SecondIndex, Quantity, Container);
}

void USFSInventoryManager::AttemptItemPickup(ASFSWorldItemActor* ItemActor)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		Server_AttemptItemPickup(ItemActor);
		return;
	}

	if (AddItemToInventory(ItemActor->GetItemClass()))
	{
		// ItemActor->Destroy();
	}
}

FInventoryStruct& USFSInventoryManager::GetInventoryStruct(int32 IndexRef)
{
	return InvArray[IndexRef];
}

void USFSInventoryManager::Server_PerformInventoryAction_Implementation(EInventoryAction InventoryAction, int32 FirstIndex, int32 SecondIndex /*= -1*/, int32 Quantity /*= 1*/, AActor* Container /*= nullptr*/)
{
	
}

bool USFSInventoryManager::Server_PerformInventoryAction_Validate(EInventoryAction InventoryAction, int32 FirstIndex, int32 SecondIndex /*= -1*/, int32 Quantity /*= 1*/, AActor* Container /*= nullptr*/)
{
	return true;
}