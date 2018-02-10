// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SFSInteractionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SFSG_API USFSInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USFSInteractionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	bool bHasCharacterOwner;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Interact();

	class ASFSCharacter* OwnerCharacter;

	FHitResult HitResult;

	bool bTraceHit;

	void LineTrace();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Interact();
		
	
};
