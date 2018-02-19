// Fill out your copyright notice in the Description page of Project Settings.

#include "SFSSplitStackWidget.h"
#include "SFSItemSlot.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"



void USFSSplitStackWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SubmitButton->OnClicked.AddDynamic(this, &USFSSplitStackWidget::SendSplitRequest);
	QuantityField->OnTextChanged.AddDynamic(this, &USFSSplitStackWidget::CheckTextField);
}

void USFSSplitStackWidget::SendSplitRequest()
{
	if (!ItemSlot) 
	{ 
		SetVisibility(ESlateVisibility::Hidden); 
		return;
	}

	FString QuantityString = QuantityField->GetText().ToString();
	if (QuantityString.IsNumeric())
	{
		int32 QuantityToSplit = FCString::Atoi(*QuantityString);
		ItemSlot->ReceiveSplitRequest(QuantityToSplit);
	}
	SetVisibility(ESlateVisibility::Hidden);
	ItemSlot = nullptr;
	StackSize = 0;
	LastGoodEntry = "";
}

void USFSSplitStackWidget::CheckTextField(const FText& Text)
{
	FString QuantityString = Text.ToString();
	if (QuantityString == LastGoodEntry)
	{
		return;
	}

	if (QuantityString.IsNumeric())
	{
		int32 FieldValue = FCString::Atoi(*QuantityString);
		if (FieldValue >= StackSize)
		{
			QuantityString = FString::FromInt(StackSize - 1);
			LastGoodEntry = QuantityString;
		}
		else if (FieldValue <= 0)
		{
			QuantityString = FString::FromInt(1);
			LastGoodEntry = QuantityString;
		}
	}
	else
	{
		QuantityString = LastGoodEntry;
	}
	UE_LOG(LogTemp, Log, TEXT("%S"), *QuantityString)
	QuantityField->SetText(FText::FromString(LastGoodEntry));
}

void USFSSplitStackWidget::SplitStackEvent(UUserWidget* InstigatingWidget, int32 ItemQuantity)
{
	USFSItemSlot* ItemSlotWidget = Cast<USFSItemSlot>(InstigatingWidget);
	if (ItemSlotWidget)
	{
		ItemSlot = ItemSlotWidget;
	}
	LastGoodEntry = "1";
	QuantityField->SetText(FText::FromString(LastGoodEntry));
	StackSize = ItemQuantity;
}
