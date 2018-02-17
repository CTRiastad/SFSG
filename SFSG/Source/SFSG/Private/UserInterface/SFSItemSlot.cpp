// Fill out your copyright notice in the Description page of Project Settings.

#include "SFSItemSlot.h"
#include "SFSItemDragDrop.h"
#include "SFSInventoryManager.h"
#include "SFSItemBase.h"


bool USFSItemSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	USFSItemDragDrop* ItemDragDrop = Cast<USFSItemDragDrop>(InOperation);
	if (ItemDragDrop)
	{
		FInventoryActionData ActionData;
		ActionData.FirstIndexRef = ItemDragDrop->ItemSlot; 
		ActionData.SecondIndexRef = ItemSlot;
		if (InventoryManager->GetInventoryStruct(ItemSlot).ItemInstance && InventoryManager->GetInventoryStruct(ItemSlot).ItemInstance->GetClass()->IsChildOf(InventoryManager->GetInventoryStruct(ItemDragDrop->ItemSlot).ItemInstance->GetClass()))
		{
			ActionData.InventoryAction = EInventoryAction::CombineStack;
		}
		else
		{
			ActionData.InventoryAction = EInventoryAction::MoveItem;
		}
		InventoryManager->PerformInventoryAction(ActionData);
		return true;
	}
	return false;
}

FReply USFSItemSlot::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		FInventoryActionData ActionData;
		ActionData.FirstIndexRef = ItemSlot;
		ActionData.InventoryAction = EInventoryAction::Use;
		InventoryManager->PerformInventoryAction(ActionData);
		return FReply::Handled();
	}
	
	return FReply::Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
}

void USFSItemSlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	if (InventoryManager->GetInventoryStruct(ItemSlot).ItemInstance)
	{
		USFSItemDragDrop* SomeDragDrop = NewObject<USFSItemDragDrop>();
		SomeDragDrop->DefaultDragVisual = this;
		SomeDragDrop->ItemSlot = ItemSlot;
		OutOperation = SomeDragDrop;
	}
}

