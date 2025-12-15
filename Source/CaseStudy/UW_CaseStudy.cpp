// Fill out your copyright notice in the Description page of Project Settings.


#include "UW_CaseStudy.h"
#include "Components/Image.h"

void UUW_CaseStudy::UpdateSlotImage(int32 SlotIndex, UTexture2D* NewIcon)
{
    UImage* TargetImage = nullptr;

    if (SlotIndex == 0)
    {
        TargetImage = ItemSlot1;
    }
    else if (SlotIndex == 1)
    {
        TargetImage = ItemSlot2;
    }

    if (TargetImage)
    {
        if (NewIcon)
        {
            TargetImage->SetBrushFromTexture(NewIcon);
            TargetImage->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
        }
        else
        {
            TargetImage->SetBrushFromTexture(nullptr);
            TargetImage->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0.f));
        }
    }
}
