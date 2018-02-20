// Fill out your copyright notice in the Description page of Project Settings.

#include "SFSContainerWorldActor.h"
#include "SFSCharacter.h"
#include "SFSItemBase.h"
#include "SFSInventoryManager.h"



void ASFSContainerWorldActor::InitializeArray()
{
	for (int32 i = 0; i < InventorySize; i++)
	{
		InvArray[i].ItemInstance = nullptr;
		InvArray[i].Quantity = 0;
	}
}

bool ASFSContainerWorldActor::OnInteractionEvent_Implementation(AActor* InteractingActor)
{
	ASFSCharacter* InteractingChar = Cast<ASFSCharacter>(InteractingActor);
	if (InteractingChar)
	{
		InteractingChar->GetInventoryComp()->AccessContainer(this);
	}
	return true;
}


