// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class CASESTUDY_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public: 
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float SpeedCPP;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float DirectionCPP;
};
