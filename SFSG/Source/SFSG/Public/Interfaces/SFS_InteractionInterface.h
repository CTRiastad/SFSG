// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SFS_InteractionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USFS_InteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SFSG_API ISFS_InteractionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	bool OnInteractionEvent(AActor* InteractingActor);

};
