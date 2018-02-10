// Fill out your copyright notice in the Description page of Project Settings.

#include "SFSWorldItemActor.h"
#include "Components/StaticMeshComponent.h"
#include "Items/SFSItemBase.h"
#include "SFSCharacter.h"
#include "SFSInventoryManager.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASFSWorldItemActor::ASFSWorldItemActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));

}

// Called when the game starts or when spawned
void ASFSWorldItemActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASFSWorldItemActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (ItemClass)
	{
		USFSItemBase* ItemBase = ItemClass->GetDefaultObject<USFSItemBase>();
		if (ItemBase)
		{
			MeshComp->SetStaticMesh(ItemBase->GetWorldMesh());
		}
	}
}

TSubclassOf<USFSItemBase> ASFSWorldItemActor::GetItemClass()
{
	return ItemClass;
}

bool ASFSWorldItemActor::OnInteractionEvent_Implementation(AActor* InteractingActor)
{

	ASFSCharacter* InteractingChar = Cast<ASFSCharacter>(InteractingActor);
	if (InteractingChar)
	{
		InteractingChar->GetInventoryComp()->AttemptItemPickup(this);
	}
	return true;
}

void ASFSWorldItemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASFSWorldItemActor, ItemClass);
}