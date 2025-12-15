// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "ItemBase.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "PlayerAnimInstance.h"
#include "UW_CaseStudy.h"
#include "Net/UnrealNetwork.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetMesh(), FName("head"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	SpringArm->bUsePawnControlRotation = true;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(PlayerMappingContext, 0);
		}
	}
	if (HasAuthority())
	{
		Inventory.Init(nullptr, 2);
		ActiveSlotIndex = 0;
	}
	if (IsLocallyControlled() && HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UUW_CaseStudy>(GetWorld(), HUDWidgetClass);
		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
		}
	}
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerCharacter, Inventory);
	DOREPLIFETIME(APlayerCharacter, ActiveSlotIndex);
	DOREPLIFETIME(APlayerCharacter, bIsHandEmpty);
}

void APlayerCharacter::OnRep_Inventory()
{
	UpdateHandVisuals();
}

void APlayerCharacter::OnRep_IsHandEmpty()
{
	UpdateHandVisuals();
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		EnhancedInputComponent->BindAction(DropAction, ETriggerEvent::Started, this, &APlayerCharacter::Drop);
		EnhancedInputComponent->BindAction(EmptyHandAction, ETriggerEvent::Started, this, &APlayerCharacter::EmptyHand);
		EnhancedInputComponent->BindAction(PickupAction, ETriggerEvent::Started, this, &APlayerCharacter::Pickup);
		EnhancedInputComponent->BindAction(Slot1Action, ETriggerEvent::Started, this, &APlayerCharacter::EquipSlot1);
		EnhancedInputComponent->BindAction(Slot2Action, ETriggerEvent::Started, this, &APlayerCharacter::EquipSlot2);
	}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector CameraForward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector CameraRight = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(CameraForward, MovementVector.Y);
		AddMovementInput(CameraRight, MovementVector.X);
	}
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(-LookAxisVector.Y);
	}
}

void APlayerCharacter::Drop()
{
	FVector SpawnLoc = Camera->GetComponentLocation() + (Camera->GetForwardVector() * 100.f);
	FRotator SpawnRot = Camera->GetComponentRotation();
	Server_DropItem(SpawnLoc, SpawnRot);
}

void APlayerCharacter::Server_DropItem_Implementation(FVector SpawnLocation, FRotator SpawnRotation)
{
	if (Inventory.IsValidIndex(ActiveSlotIndex) && Inventory[ActiveSlotIndex] != nullptr && !bIsHandEmpty)
	{
		AItemBase* ItemToDrop = Inventory[ActiveSlotIndex];

		Multicast_PlayDropAnim();

		FTimerHandle DropHandle;
		FTimerDelegate DropDelegate;

		DropDelegate.BindLambda([this, ItemToDrop, SpawnLocation, SpawnRotation]()
		{
			if (!ItemToDrop) return;

			Multicast_FinishDropItem(ItemToDrop, SpawnLocation, SpawnRotation);

			UStaticMeshComponent* ItemMesh = Cast<UStaticMeshComponent>(ItemToDrop->GetRootComponent());
			if (ItemMesh)
			{
				ItemMesh->SetSimulatePhysics(true);
				ItemMesh->AddImpulse(SpawnRotation.Vector() * 500.f, NAME_None, true);
			}
			ItemToDrop->ForceNetUpdate();

			if (Inventory.IsValidIndex(ActiveSlotIndex))
			{
				Inventory[ActiveSlotIndex] = nullptr;
				OnRep_Inventory();
			}
		});

		GetWorld()->GetTimerManager().SetTimer(DropHandle, DropDelegate, 0.8f, false);
	}
}

void APlayerCharacter::Multicast_PlayDropAnim_Implementation()
{
	PlayerAnimInstanceRef = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	if (ThrowMontage)
	{
		PlayerAnimInstanceRef->Montage_Play(ThrowMontage);
	}
}

void APlayerCharacter::Pickup()
{
	FHitResult PickupHitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	FVector TraceStart = Camera->GetComponentLocation();
	FVector TraceEnd = TraceStart + (Camera->GetForwardVector() * 400.f);
	GetWorld()->LineTraceSingleByChannel(PickupHitResult, TraceStart, TraceEnd, ECC_Visibility, Params);
	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 2.f, 1.f, 1.f);
	if (PickupHitResult.bBlockingHit)
	{
		if (AItemBase* ItemToPickup = Cast<AItemBase>(PickupHitResult.GetActor()))
		{
			Server_PickupItem(ItemToPickup);
		}
	}
}

void APlayerCharacter::Server_PickupItem_Implementation(AItemBase* ItemToPickup)
{
	if (!ItemToPickup) return;

	if (ItemToPickup->GetOwner() != nullptr)
	{
		return;
	}

	int32 TargetSlot = -1;
	if (Inventory[ActiveSlotIndex] == nullptr)
	{
		TargetSlot = ActiveSlotIndex;
	}
	else
	{
		int32 OtherSlot = (ActiveSlotIndex + 1) % 2;
		if (Inventory[OtherSlot] == nullptr) TargetSlot = OtherSlot;
	}

	if (TargetSlot != -1)
	{
		Inventory[TargetSlot] = ItemToPickup;
		ItemToPickup->SetOwner(this);

		if (TargetSlot == ActiveSlotIndex)
		{
			bIsHandEmpty = false;
		}

		OnRep_Inventory();
		Multicast_PlayPickupAnim();
	}
}

void APlayerCharacter::Multicast_PlayPickupAnim_Implementation()
{
	PlayerAnimInstanceRef = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	if (PickupMontage)
	{
		PlayerAnimInstanceRef->Montage_Play(PickupMontage);
	}
}

void APlayerCharacter::EquipSlot1()
{
	Server_SwitchSlot(0);
}

void APlayerCharacter::EquipSlot2()
{
	Server_SwitchSlot(1);
}

void APlayerCharacter::Server_SwitchSlot_Implementation(int32 NewSlotIndex)
{
	if (NewSlotIndex < 0 || NewSlotIndex > 1) return;

	if (ActiveSlotIndex == NewSlotIndex && !bIsHandEmpty) return;

	ActiveSlotIndex = NewSlotIndex;

	bIsHandEmpty = false;

	OnRep_ActiveSlotIndex();
}

void APlayerCharacter::OnRep_ActiveSlotIndex()
{
	UpdateHandVisuals();
}

void APlayerCharacter::UpdateHandVisuals()
{
	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		AItemBase* Item = Inventory[i];
		if (HUDWidget)
		{
			if (Item)
			{
				if (Item->GetItemIcon())
				{
					HUDWidget->UpdateSlotImage(i, Item->GetItemIcon());
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("HATA Slot %d: %s iteminin IKONU (Texture) yok! BP'yi kontrol et."), i, *Item->GetName());
					HUDWidget->UpdateSlotImage(i, nullptr);
				}
			}
			else
			{
				HUDWidget->UpdateSlotImage(i, nullptr);
			}
		}
		else
		{
			if (IsLocallyControlled())
			{
				UE_LOG(LogTemp, Error, TEXT("KRITIK HATA: HUDWidget NULL! BeginPlay icinde Widget olusmamis veya Class secilmemis."));
			}
		}
		if (Item)
		{
			UStaticMeshComponent* ItemMesh = Cast<UStaticMeshComponent>(Item->GetRootComponent());
			if (ItemMesh)
			{
				ItemMesh->SetSimulatePhysics(false);
				ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}

			if (i == ActiveSlotIndex && !bIsHandEmpty)
			{
				Item->SetActorHiddenInGame(false);
				Item->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("HandSocket"));
			}
			else
			{
				Item->SetActorHiddenInGame(true);
			}
		}
	}
}

void APlayerCharacter::Multicast_FinishDropItem_Implementation(AItemBase* ItemToDrop, FVector Location, FRotator Rotation)
{
	if (!ItemToDrop) return;

	ItemToDrop->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	ItemToDrop->SetActorLocationAndRotation(Location, Rotation);

	ItemToDrop->SetOwner(nullptr);
	ItemToDrop->SetActorHiddenInGame(false);
	ItemToDrop->SetActorEnableCollision(true);

	UStaticMeshComponent* ItemMesh = Cast<UStaticMeshComponent>(ItemToDrop->GetRootComponent());
	if (ItemMesh)
	{
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ItemMesh->SetSimulatePhysics(false);
	}
}

void APlayerCharacter::EmptyHand()
{
	Server_ToggleEmptyHand();
}

void APlayerCharacter::Server_ToggleEmptyHand_Implementation()
{
	bIsHandEmpty = !bIsHandEmpty;

	OnRep_IsHandEmpty();
}
