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
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayPickupAnim();
	UFUNCTION(Server, Reliable)
	void Server_DropItem(FVector SpawnLocation, FRotator SpawnRotation);
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayDropAnim();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(ReplicatedUsing = OnRep_Inventory)
	TArray<class AItemBase*> Inventory;

	UPROPERTY(ReplicatedUsing = OnRep_ActiveSlotIndex)
	int32 ActiveSlotIndex;

	UFUNCTION()
	void OnRep_ActiveSlotIndex();

	void UpdateHandVisuals();

	UFUNCTION(Server, Reliable)
	void Server_SwitchSlot(int32 NewSlotIndex);

	UFUNCTION()
	void OnRep_Inventory();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_FinishDropItem(AItemBase* ItemToDrop, FVector Location, FRotator Rotation);

	UPROPERTY(ReplicatedUsing = OnRep_IsHandEmpty)
	bool bIsHandEmpty;

	UFUNCTION()
	void OnRep_IsHandEmpty();

	UFUNCTION(Server, Reliable)
	void Server_ToggleEmptyHand();
private:

	UPROPERTY()
	class UPlayerAnimInstance* PlayerAnimInstanceRef;


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
	UInputAction* Slot1Action;
	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Slot2Action;


	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class UCameraComponent* Camera;

	
	UPROPERTY(EditAnywhere, Category = "Animations", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* PickupMontage;

	UPROPERTY(EditAnywhere, Category = "Animations", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ThrowMontage;


	UPROPERTY(EditAnywhere, Category = "ThrowObject", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AItemBase> ItemBaseClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUW_CaseStudy> HUDWidgetClass;
	UPROPERTY()
	class UUW_CaseStudy* HUDWidget;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Drop();
	void EmptyHand();
	void Pickup();
	void EquipSlot1();
	void EquipSlot2();
};
