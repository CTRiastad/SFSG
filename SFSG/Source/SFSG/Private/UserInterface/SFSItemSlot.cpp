// Fill out your copyright notice in the Description page of Project Settings.

#include "SFSItemSlot.h"
#include "SFSItemDragDrop.h"
#include "SFSInventoryManager.h"
#include "SFSItemBase.h"
#include "SFSSplitStackWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

#define LOCTEXT_NAMESPACE "ItemCountKey"


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
	FInventoryActionData ActionData;
	ActionData.FirstIndexRef = ItemSlot;
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		ActionData.InventoryAction = EInventoryAction::Use;
		InventoryManager->PerformInventoryAction(ActionData);
		return FReply::Handled();
	}
	else if (InMouseEvent.IsShiftDown() && (InventoryManager->GetInventoryStruct(ItemSlot).ItemInstance && InventoryManager->GetInventoryStruct(ItemSlot).Quantity > 1))
	{
		SplitStackWidget->SetVisibility(ESlateVisibility::Visible);
		SplitStackWidget->SplitStackEvent(this, InventoryManager->GetInventoryStruct(ItemSlot).Quantity);
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

void USFSItemSlot::UpdateWidget()
{
	if (!InventoryManager)
	{
		return;
	}

	if (InventoryManager->GetInventoryStruct(ItemSlot).ItemInstance)
	{
		FInvData InvData = InventoryManager->GetInventoryStruct(ItemSlot).ItemInstance->GetInvData();
		ItemIcon->SetBrushFromTexture(InvData.Icon);
		ItemIcon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		
		if (InventoryManager->GetInventoryStruct(ItemSlot).Quantity > 1) 
		{
			FFormatNamedArguments Arguments;
			Arguments.Add(TEXT("ItemQuantity"), FText::AsNumber(InventoryManager->GetInventoryStruct(ItemSlot).Quantity));
			ItemCount->SetText(FText::Format(LOCTEXT("ItemCountKey", "x{ItemQuantity}"), Arguments));
			ItemCount->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
		{
			ItemCount->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	else
	{
		ItemIcon->SetVisibility(ESlateVisibility::Hidden);
		ItemCount->SetVisibility(ESlateVisibility::Hidden);
	}
}

void USFSItemSlot::ReceiveSplitRequest(int32 QuantityToSplit)
{
	if (QuantityToSplit > 0)
	{
		FInventoryActionData ActionData;
		ActionData.InventoryAction = EInventoryAction::SplitStack;
		ActionData.FirstIndexRef = ItemSlot;
		ActionData.Quantity = QuantityToSplit;

		InventoryManager->PerformInventoryAction(ActionData);
	}
}

#undef LOCTEXT_NAMESPACE