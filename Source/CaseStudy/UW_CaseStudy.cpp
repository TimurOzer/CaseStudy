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

void UUW_CaseStudy::SetActiveSlotHighlight(int32 ActiveSlotIndex)
{
    FLinearColor SelectedColor = FLinearColor::Yellow;
    FLinearColor DefaultColor = FLinearColor(1.f, 1.f, 1.f, 1.f);

    if (ItemSlot1Select && ItemSlot2Select)
    {
        if (ActiveSlotIndex == 0)
        {
            ItemSlot1Select->SetColorAndOpacity(SelectedColor);
            ItemSlot2Select->SetColorAndOpacity(DefaultColor);
        }
        else if (ActiveSlotIndex == 1)
        {
            ItemSlot1Select->SetColorAndOpacity(DefaultColor);
            ItemSlot2Select->SetColorAndOpacity(SelectedColor);
        }
    }
}