// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/SFS_InteractionInterface.h"
#include "SFSWorldItemActor.generated.h"

class USFSItemBase;

UCLASS()
class SFSG_API ASFSWorldItemActor : public AActor, public ISFS_InteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASFSWorldItemActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Item")
	TSubclassOf<class USFSItemBase> ItemClass;

	virtual void OnConstruction(const FTransform& Transform) override;

	class UStaticMeshComponent* MeshComp;

public:	
	
	TSubclassOf<USFSItemBase> GetItemClass();

	virtual bool OnInteractionEvent_Implementation(AActor* InteractingActor) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
	void OnInteracted(AActor* InteractingActor);

};
