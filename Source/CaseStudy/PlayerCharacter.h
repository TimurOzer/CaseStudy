// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class CASESTUDY_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void Server_PickupItem(class AItemBase* ItemToPickup);

private:

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* PlayerMappingContext;
	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DropAction;
	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* EmptyHandAction;
	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PickupAction;
	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapSlotAction;

	UPROPERTY(EditAnywhere, Category = "Camera")
	class USpringArmComponent* SpringArm;
	UPROPERTY(EditAnywhere, Category = "Camera")
	class UCameraComponent* Camera;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Drop();
	void EmptyHand();
	void Pickup();
	void SwapSlot();
};
