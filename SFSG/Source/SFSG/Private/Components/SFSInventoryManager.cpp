// Fill out your copyright notice in the Description page of Project Settings.

#include "SFSInventoryManager.h"
#include "SFSItemBase.h"

// Sets default values for this component's properties
USFSInventoryManager::USFSInventoryManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;


}


// Called when the game starts
void USFSInventoryManager::BeginPlay()
{
	Super::BeginPlay();

	InitializeArray();
	
}

void USFSInventoryManager::InitializeArray()
{

}

void USFSInventoryManager::ClearElementAtIndex(int32 IndexRef)
{
	InvArray[IndexRef].ItemInstance = nullptr;

}

class USFSItemBase* USFSInventoryManager::GenerateItemObject(TSubclassOf<USFSItemBase> ItemClass)
{
	return NewObject<USFSItemBase>(this, ItemClass);
}

