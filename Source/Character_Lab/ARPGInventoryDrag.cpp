#include "ARPGInventoryDrag.h"
#include "ARPGPlayerController.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"

void UARPGInventoryDrag::Drop_Implementation(const FPointerEvent& PointerEvent)
{
    if (Controller.IsValid()) Controller->SetInventoryDragActive(false);
    Super::Drop_Implementation(PointerEvent);
}
void UARPGInventoryDrag::DragCancelled_Implementation(const FPointerEvent& PointerEvent)
{
    // Cancellation (including Escape or release outside the game window) never drops an item.
    if (Controller.IsValid()) Controller->SetInventoryDragActive(false);
    Super::DragCancelled_Implementation(PointerEvent);
}
void UARPGWorldDropTarget::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    UBorder* Surface = WidgetTree->ConstructWidget<UBorder>();
    Surface->SetBrushColor(FLinearColor::Transparent);
    WidgetTree->RootWidget = Surface;
}
bool UARPGWorldDropTarget::NativeOnDrop(const FGeometry& Geometry, const FDragDropEvent& Event, UDragDropOperation* Operation)
{
    UARPGInventoryDrag* Drag = Cast<UARPGInventoryDrag>(Operation);
    AARPGPlayerController* PC = Cast<AARPGPlayerController>(GetOwningPlayer());
    if (!Drag || !PC || Drag->Controller.Get() != PC) return false;
    PC->DropInventoryItem(Drag->ItemId, Drag->Source.Get());
    return true;
}
