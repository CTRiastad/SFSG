// Fill out your copyright notice in the Description page of Project Settings.

#include "SFSInventoryManager.h"
#include "SFSWorldItemActor.h"
#include "SFSItemBase.h"
#include "Net/UnrealNetwork.h"

static int32 DebugDestroyWorldItem = 0;
FAutoConsoleVariableRef CVARDebugDestroyWorldItem(
	TEXT("SFS.ConsumePickup"),
	DebugDestroyWorldItem,
	TEXT("Toggles the destruction of world item actors on pickup"),
	ECVF_Default  	// ECVF_Cheat
);

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
		ClientUpdate(IndexRef);
		return true;
	}

	if (FindEmptySlot(IndexRef))
	{
		InvArray[IndexRef].ItemInstance = GenerateItemObject(ItemToAdd->GetClass());
		InvArray[IndexRef].Quantity = QuantityToAdd;
		ClientUpdate(IndexRef);
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
		ClientUpdate(IndexRef);
		return true;
	}

	if (FindEmptySlot(IndexRef))
	{
		InvArray[IndexRef].ItemInstance = GenerateItemObject(ItemClassToAdd);
		InvArray[IndexRef].Quantity = QuantityToAdd;
		ClientUpdate(IndexRef);
		return true;
	}

	return false;
}

// If no quantity is specified, default behavior clears the array element.
void USFSInventoryManager::RemoveItemFromInventory(int32 IndexRef, int32 QuantityToRemove /*= 0*/)
{
	if (InvArray[IndexRef].ItemInstance)
	{
		if (QuantityToRemove == 0)
		{
			ClearElementAtIndex(IndexRef);
			return;
		}

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

void USFSInventoryManager::SwapItems(int32 FirstIndexRef, int32 SecondIndexRef)
{
	InvArray.Swap(FirstIndexRef, SecondIndexRef);
}

bool USFSInventoryManager::UseItem(int32 IndexRef)
{
	if (InvArray[IndexRef].ItemInstance)
	{
		InvArray[IndexRef].ItemInstance->OnUsed();
		RemoveItemFromInventory(IndexRef, 1);  // TODO - Remove magic number
		return true; // TODO - Add in Usable item check
	}
	return false;
}

void USFSInventoryManager::CombineStack(int32 FirstIndexRef, int32 SecondIndexRef)
{
	FInventoryStruct& DonorStack = InvArray[FirstIndexRef];
	FInventoryStruct& TargetStack = InvArray[SecondIndexRef];

	if (DonorStack.ItemInstance->GetClass()->IsChildOf(TargetStack.ItemInstance->GetClass()) && DonorStack.ItemInstance->GetMaxStackSize() > 1)
	{
		int32 RoomInStack = TargetStack.ItemInstance->GetMaxStackSize() - TargetStack.Quantity;
		if (DonorStack.Quantity <= RoomInStack)
		{
			TargetStack.Quantity += DonorStack.Quantity;
			RemoveItemFromInventory(FirstIndexRef);
		}
		else
		{
			TargetStack.Quantity += RoomInStack;
			DonorStack.Quantity -= RoomInStack;
		}
	}
}

// The client-side actions of SplitStack are already covered by existing functions which makes the creation of a Client_SplitStack RPC superfluous.
void USFSInventoryManager::SplitStack(int32 IndexRef, int32 QuantityToSplit)
{
	int32 NewIndexRef;
	if (FindEmptySlot(NewIndexRef))
	{
		InvArray[NewIndexRef].ItemInstance = InvArray[IndexRef].ItemInstance;
		InvArray[NewIndexRef].Quantity = QuantityToSplit;
		RemoveItemFromInventory(IndexRef, QuantityToSplit);
	}
}

void USFSInventoryManager::Client_AttemptItemPickup_Implementation(class ASFSWorldItemActor* ItemActor)
{
	AttemptItemPickup(ItemActor);
}

void USFSInventoryManager::Client_PerformInventoryAction_Implementation(const FInventoryActionData& ActionRequest)
{
	switch (ActionRequest.InventoryAction)
	{
	case EInventoryAction::MoveItem:
		SwapItems(ActionRequest.FirstIndexRef, ActionRequest.SecondIndexRef);
		UpdateClientEvent.Broadcast(ActionRequest.FirstIndexRef);
		UpdateClientEvent.Broadcast(ActionRequest.SecondIndexRef);
		break;
	case EInventoryAction::RemoveItem:
		RemoveItemFromInventory(ActionRequest.FirstIndexRef, ActionRequest.Quantity);
		UpdateClientEvent.Broadcast(ActionRequest.FirstIndexRef);
		break;
	case EInventoryAction::SplitStack:
		SplitStack(ActionRequest.FirstIndexRef, ActionRequest.Quantity);
		UpdateClientEvent.Broadcast(ActionRequest.FirstIndexRef);
		UpdateClientEvent.Broadcast(ActionRequest.SecondIndexRef);
		break;
	case EInventoryAction::Use:
		UseItem(ActionRequest.FirstIndexRef);
		UpdateClientEvent.Broadcast(ActionRequest.FirstIndexRef);
		break;
	case EInventoryAction::CombineStack:
		CombineStack(ActionRequest.FirstIndexRef, ActionRequest.SecondIndexRef);
		UpdateClientEvent.Broadcast(ActionRequest.FirstIndexRef);
		UpdateClientEvent.Broadcast(ActionRequest.SecondIndexRef);
		break;
	default:
		break;
	}
	UE_LOG(LogTemp, Log, TEXT("ClientInvAction"))
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
		return InvItem.ItemInstance ? (InvItem.ItemInstance->GetClass()->IsChildOf(ItemClassToAdd)) && (InvItem.Quantity + QuantityToAdd <= InvItem.ItemInstance->GetMaxStackSize()) : false;
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
	case EInventoryAction::Use:
		UseItem(ActionRequest.FirstIndexRef);
		break;
	case EInventoryAction::CombineStack:
		CombineStack(ActionRequest.FirstIndexRef, ActionRequest.SecondIndexRef);
		break;
	default:
		break;
	}
	UE_LOG(LogTemp, Log, TEXT("ServerInvAction"))
	Client_PerformInventoryAction(ActionRequest);
}

void USFSInventoryManager::Server_PerformInventoryAction_Implementation(const FInventoryActionData& ActionRequest)
{
	PerformInventoryAction(ActionRequest);
}

bool USFSInventoryManager::Server_PerformInventoryAction_Validate(const FInventoryActionData& ActionRequest)
{
	return true;
}

void USFSInventoryManager::AttemptItemPickup(ASFSWorldItemActor* ItemActor)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		Client_AttemptItemPickup(ItemActor);
	}

	if (AddItemToInventory(ItemActor->GetItemClass()))
	{

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