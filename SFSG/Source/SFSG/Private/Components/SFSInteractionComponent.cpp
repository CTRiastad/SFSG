// Fill out your copyright notice in the Description page of Project Settings.

#include "SFSInteractionComponent.h"
#include "SFSCharacter.h"
#include "SFS_InteractionInterface.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

static int32 DebugTraceDrawing = 0;
FAutoConsoleVariableRef CVARDebugTraceDrawing(
	TEXT("TP.DebugInteract"),
	DebugTraceDrawing,
	TEXT("DrawDebugLines for Interaction Traces"),
	ECVF_Default  	// ECVF_Cheat
);

// Sets default values for this component's properties
USFSInteractionComponent::USFSInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USFSInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ASFSCharacter>(GetOwner());
	if (OwnerCharacter)
	{
		bHasCharacterOwner = true;
	}
	
}


void USFSInteractionComponent::Server_Interact_Implementation()
{
	Interact();
}

bool USFSInteractionComponent::Server_Interact_Validate()
{
	return true;
}

void USFSInteractionComponent::LineTrace()
{
	FVector Start;
	FVector End;
	OwnerCharacter->GetTraceVectors(Start, End);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility))
	{
		bTraceHit = true;
	}
	else
	{
		bTraceHit = false;
	}

	if (DebugTraceDrawing > 0)
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, 1.0f, 0, 2.f);
	}
}

// Called every frame
void USFSInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bHasCharacterOwner)
	{
		LineTrace();
	}
}

void USFSInteractionComponent::Interact()
{
	if (!bTraceHit)
	{
		return;
	}

	if (GetOwnerRole() < ROLE_Authority)
	{
		Server_Interact();
	}

	UObject* pointerToAnyObject = HitResult.GetActor();
	ISFS_InteractionInterface* InteractionInterface = Cast<ISFS_InteractionInterface>(pointerToAnyObject);
	if (InteractionInterface)
	{
		InteractionInterface->Execute_OnInteractionEvent(pointerToAnyObject, GetOwner());
	}
}

