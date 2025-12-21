// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"
#include "BaseCharacter.h"
#include "KismetAnimationLibrary.h"

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (TryGetPawnOwner())
	{
		ABaseCharacter* PlayerCharacterRef = Cast<ABaseCharacter>(TryGetPawnOwner());
		DirectionCPP = UKismetAnimationLibrary::CalculateDirection(TryGetPawnOwner()->GetVelocity(), TryGetPawnOwner()->GetActorRotation());
		SpeedCPP = TryGetPawnOwner()->GetVelocity().Size();
	}
}
