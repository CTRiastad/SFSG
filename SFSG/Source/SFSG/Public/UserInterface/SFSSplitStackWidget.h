// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SFSSplitStackWidget.generated.h"

/**
 * 
 */
UCLASS()
class SFSG_API USFSSplitStackWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	class USFSItemSlot* ItemSlot;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void SendSplitRequest();

	UFUNCTION()
	void CheckTextField(const FText& Text);

	FString LastGoodEntry;

	int32 StackSize;

public:

	void SplitStackEvent(UUserWidget* InstigatingWidget, int32 ItemQuantity);
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UEditableTextBox* QuantityField;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* SubmitButton;
};
