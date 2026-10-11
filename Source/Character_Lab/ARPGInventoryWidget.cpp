#include "ARPGInventoryWidget.h"
#include "ARPGInventoryComponent.h"
#include "ARPGPlayerController.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "InputCoreTypes.h"
#include "ARPGInventoryDrag.h"

void UARPGInventoryTile::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    Background = WidgetTree->ConstructWidget<UBorder>();
    Background->SetPadding(FMargin(5.f));
    Icon = WidgetTree->ConstructWidget<UImage>();
    Background->SetContent(Icon);
    WidgetTree->RootWidget = Background;
    SetVisibility(ESlateVisibility::Visible);
}
void UARPGInventoryTile::Configure(UARPGInventoryWidget* InPanel, UARPGItemDefinition* Definition, FGuid Id, bool bSelected)
{
    Panel = InPanel;
    ItemId = Id;
    Icon->SetBrush(Definition->Icon);
    Background->SetBrushColor(bSelected ? FLinearColor(0.35f, 0.26f, 0.1f, 1.f) : FLinearColor(0.09f, 0.13f, 0.18f, 1.f));
    SetToolTipText(FText::Format(NSLOCTEXT("Inventory", "Tooltip", "{0} ({1} × {2})"), Definition->DisplayName, FText::AsNumber(Definition->Size.X), FText::AsNumber(Definition->Size.Y)));
}
FReply UARPGInventoryTile::NativeOnMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& Event)
{
    if (Panel.IsValid() && Event.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        const FVector2D Local = Geometry.AbsoluteToLocal(Event.GetScreenSpacePosition());
        const FVector2D Size = Geometry.GetLocalSize();
        GrabFraction = FVector2D(Local.X / FMath::Max(1.f, static_cast<float>(Size.X)), Local.Y / FMath::Max(1.f, static_cast<float>(Size.Y)));
        // Do not rebuild/select on mouse-down: the original widget must survive drag detection.
        return FReply::Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
    }
    return FReply::Handled();
}
FReply UARPGInventoryTile::NativeOnMouseButtonUp(const FGeometry& Geometry, const FPointerEvent& Event)
{
    if (Panel.IsValid() && Event.GetEffectingButton() == EKeys::LeftMouseButton) Panel->SelectItem(ItemId);
    return FReply::Handled();
}
void UARPGInventoryTile::NativeOnDragDetected(const FGeometry& Geometry, const FPointerEvent& Event, UDragDropOperation*& Operation)
{
    if (Panel.IsValid()) Operation = Panel->BeginItemDrag(ItemId, GrabFraction);
}

void UARPGInventoryWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    UBorder* Background = WidgetTree->ConstructWidget<UBorder>();
    Background->SetBrushColor(FLinearColor(0.015f, 0.02f, 0.03f, 0.98f));
    Background->SetPadding(FMargin(14.f));
    UVerticalBox* Rows = WidgetTree->ConstructWidget<UVerticalBox>();
    Background->SetContent(Rows);
    USizeBox* PanelSize = WidgetTree->ConstructWidget<USizeBox>();
    PanelSize->SetWidthOverride(400.f);
    PanelSize->SetContent(Background);
    WidgetTree->RootWidget = PanelSize;
    auto Row = [Rows](UWidget* Child) { Rows->AddChildToVerticalBox(Child)->SetPadding(FMargin(0.f, 2.f)); };
    auto SmallText = [](UTextBlock* Text) { FSlateFontInfo Font = Text->GetFont(); Font.Size = 14; Text->SetFont(Font); };
    UTextBlock* Title = WidgetTree->ConstructWidget<UTextBlock>();
    Title->SetText(NSLOCTEXT("Inventory", "Title", "INVENTORY  [I]"));
    SmallText(Title);
    Row(Title);
    UTextBlock* EquipmentTitle = WidgetTree->ConstructWidget<UTextBlock>();
    EquipmentTitle->SetText(NSLOCTEXT("Inventory", "EquipmentTitle", "Utrustad chest"));
    SmallText(EquipmentTitle);
    Row(EquipmentTitle);
    Equipment = WidgetTree->ConstructWidget<UHorizontalBox>();
    EquipmentTarget = WidgetTree->ConstructWidget<UBorder>();
    EquipmentTarget->SetBrushColor(FLinearColor(0.06f, 0.09f, 0.12f, 1.f));
    EquipmentTarget->SetPadding(FMargin(6.f));
    EquipmentTarget->SetContent(Equipment);
    USizeBox* EquipmentBox = WidgetTree->ConstructWidget<USizeBox>();
    EquipmentBox->SetMinDesiredHeight(76.f);
    EquipmentBox->SetContent(EquipmentTarget);
    Row(EquipmentBox);
    GridBox = WidgetTree->ConstructWidget<USizeBox>();
    Grid = WidgetTree->ConstructWidget<UCanvasPanel>();
    GridBox->SetContent(Grid);
    Row(GridBox);
    Details = WidgetTree->ConstructWidget<UTextBlock>();
    SmallText(Details);
    Details->SetAutoWrapText(true);
    Row(Details);
    UButton* Action = WidgetTree->ConstructWidget<UButton>();
    ActionLabel = WidgetTree->ConstructWidget<UTextBlock>();
    SmallText(ActionLabel);
    Action->SetContent(ActionLabel);
    Action->OnClicked.AddDynamic(this, &ThisClass::ToggleEquipment);
    Row(Action);
    Status = WidgetTree->ConstructWidget<UTextBlock>();
    SmallText(Status);
    Status->SetAutoWrapText(true);
    Status->SetColorAndOpacity(FSlateColor(FLinearColor(1.f, 0.8f, 0.3f)));
    Row(Status);
    UButton* CloseButton = WidgetTree->ConstructWidget<UButton>();
    UTextBlock* CloseLabel = WidgetTree->ConstructWidget<UTextBlock>();
    CloseLabel->SetText(NSLOCTEXT("Inventory", "Close", "Stäng [I]"));
    SmallText(CloseLabel);
    CloseButton->SetContent(CloseLabel);
    CloseButton->OnClicked.AddDynamic(this, &ThisClass::Close);
    Row(CloseButton);
    SetVisibility(ESlateVisibility::Visible);
}
void UARPGInventoryWidget::BindInventory(UARPGInventoryComponent* Inventory)
{
    if (Source.IsValid()) Source->OnChanged.RemoveAll(this);
    Source = Inventory;
    if (Source.IsValid()) Source->OnChanged.AddUObject(this, &ThisClass::Refresh);
    Refresh();
}
void UARPGInventoryWidget::NativeDestruct()
{
    if (Source.IsValid()) Source->OnChanged.RemoveAll(this);
    Source.Reset();
    Super::NativeDestruct();
}
void UARPGInventoryWidget::Refresh()
{
    if (!Grid || !Equipment) return;
    Grid->ClearChildren();
    Equipment->ClearChildren();
    if (!Source.IsValid())
    {
        SelectedId.Invalidate();
        Details->SetText(NSLOCTEXT("Inventory", "NoInventory", "Inget inventory tillgängligt."));
        ActionLabel->SetText(NSLOCTEXT("Inventory", "Equip", "Utrusta valt föremål"));
        return;
    }
    GridBox->SetWidthOverride(Source->GridSize.X * CellSize);
    GridBox->SetHeightOverride(Source->GridSize.Y * CellSize);
    for (int32 Y = 0; Y < Source->GridSize.Y; ++Y)
        for (int32 X = 0; X < Source->GridSize.X; ++X)
        {
            UBorder* Cell = WidgetTree->ConstructWidget<UBorder>();
            Cell->SetBrushColor(FLinearColor(0.035f, 0.045f, 0.06f, 1.f));
            UCanvasPanelSlot* CellSlot = Grid->AddChildToCanvas(Cell);
            CellSlot->SetPosition(FVector2D(X * CellSize, Y * CellSize));
            CellSlot->SetSize(FVector2D(CellSize - 1.f, CellSize - 1.f));
        }
    for (const FARPGInventoryItem& Entry : Source->GetItems())
    {
        if (!Entry.Definition) continue;
        UARPGInventoryTile* Tile = CreateWidget<UARPGInventoryTile>(GetOwningPlayer());
        Tile->Configure(this, Entry.Definition, Entry.Id, Entry.Id == SelectedId);
        if (Entry.bEquipped)
        {
            USizeBox* Box = WidgetTree->ConstructWidget<USizeBox>();
            Box->SetWidthOverride(48.f); Box->SetHeightOverride(64.f);
            Box->SetContent(Tile);
            Equipment->AddChildToHorizontalBox(Box);
        }
        else
        {
            UCanvasPanelSlot* ItemSlot = Grid->AddChildToCanvas(Tile);
            ItemSlot->SetPosition(FVector2D(Entry.Position.X * CellSize, Entry.Position.Y * CellSize));
            ItemSlot->SetSize(FVector2D(Entry.Definition->Size.X * CellSize - 1.f, Entry.Definition->Size.Y * CellSize - 1.f));
            ItemSlot->SetZOrder(1);
        }
    }
    if (Equipment->GetChildrenCount() == 0)
    {
        UTextBlock* Empty = WidgetTree->ConstructWidget<UTextBlock>();
        Empty->SetText(NSLOCTEXT("Inventory", "EmptySlot", "Tom"));
        Equipment->AddChildToHorizontalBox(Empty);
    }
    const FARPGInventoryItem* Selected = Source->FindItem(SelectedId);
    Details->SetText(Selected ? FText::Format(NSLOCTEXT("Inventory", "Selected", "{0} — {1} × {2} rutor"), Selected->Definition->DisplayName, FText::AsNumber(Selected->Definition->Size.X), FText::AsNumber(Selected->Definition->Size.Y)) : NSLOCTEXT("Inventory", "SelectHelp", "Välj en ikon. Klicka i en tom ruta för att flytta den."));
    ActionLabel->SetText(Selected && Selected->bEquipped ? NSLOCTEXT("Inventory", "Unequip", "Ta av till inventory") : NSLOCTEXT("Inventory", "Equip", "Utrusta valt föremål"));
}
void UARPGInventoryWidget::StopMovement()
{
    if (AARPGPlayerController* PC = Cast<AARPGPlayerController>(GetOwningPlayer())) PC->StopMovementForUI();
}
void UARPGInventoryWidget::SelectItem(FGuid Id)
{
    StopMovement();
    SelectedId = Id;
    ShowMessage(FText::GetEmpty());
    Refresh();
}
void UARPGInventoryWidget::ShowMessage(const FText& Message) { if (Status) Status->SetText(Message); }
UDragDropOperation* UARPGInventoryWidget::BeginItemDrag(FGuid Id, FVector2D GrabFraction)
{
    StopMovement();
    if (!Source.IsValid()) return nullptr;
    const FARPGInventoryItem* Entry = Source->FindItem(Id);
    AARPGPlayerController* PC = Cast<AARPGPlayerController>(GetOwningPlayer());
    if (!Entry || !Entry->Definition || !PC) return nullptr;
    UARPGInventoryDrag* Drag = NewObject<UARPGInventoryDrag>();
    Drag->Source = Source;
    Drag->Controller = PC;
    Drag->ItemId = Id;
    const FIntPoint Size = Entry->Definition->Size;
    Drag->GrabCell = FIntPoint(FMath::Clamp(FMath::FloorToInt(GrabFraction.X * Size.X), 0, Size.X - 1), FMath::Clamp(FMath::FloorToInt(GrabFraction.Y * Size.Y), 0, Size.Y - 1));
    UARPGInventoryTile* Visual = CreateWidget<UARPGInventoryTile>(PC);
    Visual->Configure(nullptr, Entry->Definition, Id, true);
    // Drag visuals use their own desired size, not a viewport slot.
    USizeBox* VisualBox = Visual->WidgetTree->ConstructWidget<USizeBox>();
    UWidget* Content = Visual->WidgetTree->RootWidget;
    VisualBox->SetWidthOverride(Size.X * CellSize);
    VisualBox->SetHeightOverride(Size.Y * CellSize);
    VisualBox->SetContent(Content);
    Visual->WidgetTree->RootWidget = VisualBox;
    Visual->SetVisibility(ESlateVisibility::HitTestInvisible);
    Drag->DefaultDragVisual = Visual;
    Drag->Pivot = EDragPivot::TopLeft;
    Drag->Offset = -GrabFraction;
    PC->SetInventoryDragActive(true);
    ShowMessage(NSLOCTEXT("Inventory", "DragHelp", "Släpp i rutnätet, på utrustningsplatsen eller utanför panelen för markdrop."));
    return Drag;
}
bool UARPGInventoryWidget::NativeOnDrop(const FGeometry& Geometry, const FDragDropEvent& Event, UDragDropOperation* Operation)
{
    UARPGInventoryDrag* Drag = Cast<UARPGInventoryDrag>(Operation);
    if (!Drag) return false;
    if (!Source.IsValid() || Drag->Source.Get() != Source.Get()) return true;
    FText Error;
    if (EquipmentTarget->GetCachedGeometry().IsUnderLocation(Event.GetScreenSpacePosition()))
    {
        const FARPGInventoryItem* Entry = Source->FindItem(Drag->ItemId);
        if (!Entry || !Entry->bEquipped) Source->TryEquip(Drag->ItemId, Error);
    }
    else if (Grid->GetCachedGeometry().IsUnderLocation(Event.GetScreenSpacePosition()))
    {
        const FVector2D Local = Grid->GetCachedGeometry().AbsoluteToLocal(Event.GetScreenSpacePosition());
        const FIntPoint Position(FMath::FloorToInt(Local.X / CellSize), FMath::FloorToInt(Local.Y / CellSize));
        Source->TryStoreAt(Drag->ItemId, Position - Drag->GrabCell, Error);
    }
    else Error = NSLOCTEXT("Inventory", "InvalidDrop", "Släpp på en ruta eller utrustningsplatsen. Föremålet ligger kvar.");
    ShowMessage(Error);
    return true; // Invalid panel drops must not fall through to the world drop target.
}
void UARPGInventoryWidget::ToggleEquipment()
{
    StopMovement();
    if (!Source.IsValid()) return;
    const FARPGInventoryItem* Entry = Source->FindItem(SelectedId);
    if (!Entry) { ShowMessage(NSLOCTEXT("Inventory", "SelectFirst", "Välj ett föremål först.")); return; }
    FText Error;
    const bool bSuccess = Entry->bEquipped ? Source->TryUnequip(SelectedId, Error) : Source->TryEquip(SelectedId, Error);
    ShowMessage(bSuccess ? FText::GetEmpty() : Error);
}
void UARPGInventoryWidget::Close()
{
    StopMovement();
    SetVisibility(ESlateVisibility::Collapsed);
}
FReply UARPGInventoryWidget::NativeOnPreviewMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& Event)
{
    StopMovement();
    // Let the child button/tile receive the same event after stopping held world movement.
    return FReply::Unhandled();
}
FReply UARPGInventoryWidget::NativeOnMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& Event)
{
    StopMovement();
    if (Source.IsValid() && Event.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        const FVector2D Local = Grid->GetCachedGeometry().AbsoluteToLocal(Event.GetScreenSpacePosition());
        if (Local.X >= 0.f && Local.Y >= 0.f && Local.X < Source->GridSize.X * CellSize && Local.Y < Source->GridSize.Y * CellSize && SelectedId.IsValid())
        {
            FText Error;
            Source->TryMove(SelectedId, FIntPoint(FMath::FloorToInt(Local.X / CellSize), FMath::FloorToInt(Local.Y / CellSize)), Error);
            ShowMessage(Error);
        }
    }
    return FReply::Handled();
}
