// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SFSCharacter.generated.h"

UCLASS()
class SFSG_API ASFSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASFSCharacter();

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Actor component responsible for manipulation and management of inventories */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USFSInventoryManager* InventoryManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USFSInteractionComponent* InteractionComponent;

protected:

	class ASFSPlayerController* PlayerController;

	/**Called when the game starts or when spawned */
	virtual void BeginPlay() override;

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** Attempts to interact with world objects */
	void Interact();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void GetTraceVectors(FVector& Start, FVector& End);

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	/** Returns InventoryManager subobject **/
	FORCEINLINE class USFSInventoryManager* GetInventoryComp() const { return InventoryManager; }
};
