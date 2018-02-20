// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/SFSWorldItemActor.h"
#include "SFS_GlobalTypes.h"
#include "SFSContainerWorldActor.generated.h"

/**
 * 
 */
UCLASS()
class SFSG_API ASFSContainerWorldActor : public ASFSWorldItemActor
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	int32 InventorySize;

	void InitializeArray();


public:

	virtual bool OnInteractionEvent_Implementation(AActor* InteractingActor) override;

	TArray<FInventoryStruct> InvArray;
	
};
