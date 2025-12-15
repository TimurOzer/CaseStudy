// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"
#include "PlayerCharacter.h"
#include "KismetAnimationLibrary.h"

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (TryGetPawnOwner())
	{
		APlayerCharacter* PlayerCharacterRef = Cast<APlayerCharacter>(TryGetPawnOwner());
		DirectionCPP = UKismetAnimationLibrary::CalculateDirection(TryGetPawnOwner()->GetVelocity(), TryGetPawnOwner()->GetActorRotation());
		SpeedCPP = TryGetPawnOwner()->GetVelocity().Size();
	}
}
