// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "ItemBase.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

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
		EnhancedInputComponent->BindAction(SwapSlotAction, ETriggerEvent::Started, this, &APlayerCharacter::SwapSlot);
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

}

void APlayerCharacter::EmptyHand()
{

}

void APlayerCharacter::Pickup()
{
	FHitResult PickupHitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByChannel(PickupHitResult, GetActorLocation(), GetActorForwardVector() * 200.f + GetActorLocation(), ECC_Visibility, Params);
	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorForwardVector() * 200.f + GetActorLocation(), FColor::Red, false, 2.f, 1.f, 1.f);
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
	if (ItemToPickup)
	{
		ItemToPickup->Destroy();
	}
}

void APlayerCharacter::SwapSlot()
{

}
