// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SFSItemSlot.generated.h"

/**
 * 
 */
UCLASS(abstract)
class SFSG_API USFSItemSlot : public UUserWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadWrite, Category = "ItemSlot", meta = (ExposeOnSpawn = true))
	int32 ItemSlot;
	
	UPROPERTY(BlueprintReadWrite, Category = "ItemSlot", meta = (ExposeOnSpawn = true))
	class USFSInventoryManager* InventoryManager;

	UPROPERTY(BlueprintReadWrite, Category = "ItemSlot", meta = (ExposeOnSpawn = true))
	class USFSSplitStackWidget* SplitStackWidget;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

	UFUNCTION(BlueprintCallable)
	void UpdateWidget();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SplitStackResult(int32& QuantityToSplit);

public:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* ItemCount;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* ItemIcon;

	void ReceiveSplitRequest(int32 QuantityToSplit);
};
