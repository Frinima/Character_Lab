#include "ChestLootWidget.h"
#include "ARPGPlayerController.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "ARPGPlayerState.h"
#include "ARPGInventoryComponent.h"
#include "GameFramework/Pawn.h"
#include "InputCoreTypes.h"
#include "ARPGDroppedItem.h"
#include "Components/TextBlock.h"

void UChestLootWidget::SetItemName(const FText& Name)
{
    UBorder* Background = WidgetTree->ConstructWidget<UBorder>();
    Background->SetBrushColor(FLinearColor(0.f, 0.f, 0.f, 0.9f));
    Background->SetHorizontalAlignment(HAlign_Center);
    Background->SetVerticalAlignment(VAlign_Center);
    UTextBlock* Text = WidgetTree->ConstructWidget<UTextBlock>();
    Text->SetText(Name);
    Text->SetColorAndOpacity(FSlateColor(FLinearColor(1.f, 0.8f, 0.1f)));
    Background->SetContent(Text);
    WidgetTree->RootWidget = Background;
    SetVisibility(ESlateVisibility::Visible);
}

void UChestLootWidget::SetLabel(UUserWidget* Label)
{
    UBorder* HitArea = WidgetTree->ConstructWidget<UBorder>();
    HitArea->SetBrushColor(FLinearColor::Transparent);
    HitArea->SetPadding(FMargin(0.f));
    HitArea->SetContent(Label);
    WidgetTree->RootWidget = HitArea;
    SetVisibility(ESlateVisibility::Visible);
}

bool UChestLootWidget::IsAvailable() const
{
    return Item.IsValid() && Plate.IsValid() && !Plate->IsHidden();
}

FReply UChestLootWidget::NativeOnMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& Event)
{
    if (Event.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        if (AARPGPlayerController* PC = Cast<AARPGPlayerController>(GetOwningPlayer())) PC->RequestPickup(this);
        return FReply::Handled();
    }
    return Super::NativeOnMouseButtonDown(Geometry, Event);
}

bool UChestLootWidget::TryPickup(APawn* Recipient)
{
    if (!Recipient || !IsAvailable()) return false;
    AARPGPlayerController* PC = Cast<AARPGPlayerController>(Recipient->GetController());
    AARPGPlayerState* State = Recipient->GetPlayerState<AARPGPlayerState>();
    if (!PC || !State || Recipient->GetDistanceTo(Item.Get()) > PC->GetPickupDistance()) return false;
    FGuid NewItem;
    FText Error;
    if (AARPGDroppedItem* Dropped = Cast<AARPGDroppedItem>(Item.Get()))
    {
        const bool bCollected = Dropped->TryCollect(State->GetInventory(), Error);
        PC->ShowInventoryMessage(bCollected ? NSLOCTEXT("Inventory", "Collected", "Föremålet ligger i inventoryt.") : Error);
        return bCollected;
    }
    if (!State->GetInventory()->TryAdd(Definition, NewItem, Error))
    {
        PC->ShowInventoryMessage(Error);
        return false; // Leave the world item and label untouched when the inventory is full.
    }
    Item->SetActorHiddenInGame(true);
    Item->SetActorEnableCollision(false);
    Plate->SetActorHiddenInGame(true);
    if (Stem.IsValid()) Stem->SetActorHiddenInGame(true);
    PC->ShowInventoryMessage(NSLOCTEXT("Inventory", "PickedUp", "Föremålet ligger i inventoryt. Välj ikonen för att utrusta det."));
    return true;
}
