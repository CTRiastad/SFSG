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

USFSItemBase* USFSInventoryManager::GenerateItemObject(TSubclassOf<USFSItemBase> ItemClass)
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
		Client_AddItem(IndexRef, QuantityToAdd);
		return true;
	}

	if (FindEmptySlot(IndexRef))
	{
		InvArray[IndexRef].ItemInstance = GenerateItemObject(ItemToAdd->GetClass());
		InvArray[IndexRef].Quantity = QuantityToAdd;
		Client_AddItem(IndexRef, QuantityToAdd, ItemToAdd->GetClass());
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
		Client_AddItem(IndexRef, QuantityToAdd);
		return true;
	}

	if (FindEmptySlot(IndexRef))
	{
		InvArray[IndexRef].ItemInstance = GenerateItemObject(ItemClassToAdd);
		InvArray[IndexRef].Quantity = QuantityToAdd;
		Client_AddItem(IndexRef, QuantityToAdd, ItemClassToAdd);
		return true;
	}

	return false;
}

void USFSInventoryManager::Client_AddItem_Implementation(int32 IndexRef, int32 QuantityToAdd /*= 1*/, TSubclassOf<USFSItemBase> ItemClass /*= nullptr*/)
{
	if (ItemClass)
	{
		InvArray[IndexRef].ItemInstance = GenerateItemObject(ItemClass);
		InvArray[IndexRef].Quantity = QuantityToAdd;
	}
	else
	{
		InvArray[IndexRef].Quantity += QuantityToAdd;
	}


	UpdateClientEvent.Broadcast(IndexRef);
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

void USFSInventoryManager::Client_RemoveItem_Implementation(int32 IndexRef, int32 QuantityToRemove)
{
	if (InvArray[IndexRef].Quantity - QuantityToRemove > 0)
	{
		InvArray[IndexRef].Quantity -= QuantityToRemove;
	}
	else
	{
		ClearElementAtIndex(IndexRef);
	}
	UpdateClientEvent.Broadcast(IndexRef);
}

void USFSInventoryManager::SwapItems(int32 FirstIndexRef, int32 SecondIndexRef)
{
	InvArray.Swap(FirstIndexRef, SecondIndexRef);
	Client_SwapItem(FirstIndexRef, SecondIndexRef);
}

void USFSInventoryManager::Client_SwapItem_Implementation(int32 FirstIndexRef, int32 SecondIndexRef)
{
	InvArray.Swap(FirstIndexRef, SecondIndexRef);
	UpdateClientEvent.Broadcast(FirstIndexRef);
	UpdateClientEvent.Broadcast(SecondIndexRef);
}

void USFSInventoryManager::SplitStack(int32 IndexRef, int32 QuantityToSplit)
{
	int32 NewIndexRef;
	if (FindEmptySlot(NewIndexRef))
	{
		InvArray[NewIndexRef].ItemInstance = InvArray[IndexRef].ItemInstance;
		InvArray[NewIndexRef].Quantity = QuantityToSplit;
		RemoveItemFromInventory(IndexRef, QuantityToSplit);

		Client_AddItem(NewIndexRef, QuantityToSplit, InvArray[IndexRef].ItemInstance->GetClass());
		Client_RemoveItem(IndexRef, QuantityToSplit);
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

void USFSInventoryManager::PerformInventoryAction(const FInventoryActionData& ActionRequest)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		Server_PerformInventoryAction(ActionRequest);
		return;
	}

	switch (ActionRequest.InventoryAction)
	{
	case EInventoryAction::MoveItem:
		SwapItems(ActionRequest.FirstIndexRef, ActionRequest.SecondIndexRef);
		break;
	case EInventoryAction::RemoveItem:
		RemoveItemFromInventory(ActionRequest.FirstIndexRef, ActionRequest.Quantity);
		break;
	case EInventoryAction::SplitStack:
		SplitStack(ActionRequest.FirstIndexRef, ActionRequest.Quantity);
		break;
	default:
		break;
	}

}

void USFSInventoryManager::Server_PerformInventoryAction_Implementation(const FInventoryActionData& InventoryAction)
{
	PerformInventoryAction(InventoryAction);
}

bool USFSInventoryManager::Server_PerformInventoryAction_Validate(const FInventoryActionData& InventoryAction)
{
	return true;
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
		ItemActor->Destroy();
	}
}

void USFSInventoryManager::Server_AttemptItemPickup_Implementation(ASFSWorldItemActor* ItemActor)
{
	AttemptItemPickup(ItemActor);
}

bool USFSInventoryManager::Server_AttemptItemPickup_Validate(ASFSWorldItemActor* ItemActor)
{
	return true;
}

FInventoryStruct& USFSInventoryManager::GetInventoryStruct(int32 IndexRef)
{
	return InvArray[IndexRef];
}