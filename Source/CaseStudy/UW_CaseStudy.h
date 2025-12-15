// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_CaseStudy.generated.h"

/**
 * 
 */
UCLASS()
class CASESTUDY_API UUW_CaseStudy : public UUserWidget
{
	GENERATED_BODY()
public:
	void UpdateSlotImage(int32 SlotIndex, class UTexture2D* NewIcon);

private:
	UPROPERTY(meta = (BindWidget))
	class UImage* Crosshair;
	UPROPERTY(meta = (BindWidget))
	class UImage* ItemSlot1;
	UPROPERTY(meta = (BindWidget))
	class UImage* ItemSlot2;
};
