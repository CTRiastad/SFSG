// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SFSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SFSG_API ASFSPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

protected:
	
	virtual void SetupInputComponent() override;

	bool bCharacterControllable;

public:

};
