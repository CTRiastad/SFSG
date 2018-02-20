// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "SFSItemDragDrop.generated.h"

/**
 * 
 */
UCLASS()
class SFSG_API USFSItemDragDrop : public UDragDropOperation
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadWrite, Category = "DragDrop", meta=(ExposeOnSpawn=true))
	int32 ItemSlot;

	UPROPERTY(BlueprintReadWrite, Category = "DragDrop", meta = (ExposeOnSpawn = true))
	class ASFSContainerWorldActor* ContainerActor = nullptr;
};
