#pragma once
#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "Blueprint/UserWidget.h"
#include "ARPGInventoryDrag.generated.h"
class UARPGInventoryComponent;
class AARPGPlayerController;

UCLASS()
class CHARACTER_LAB_API UARPGInventoryDrag : public UDragDropOperation
{
    GENERATED_BODY()
public:
    TWeakObjectPtr<UARPGInventoryComponent> Source;
    TWeakObjectPtr<AARPGPlayerController> Controller;
    FGuid ItemId;
    FIntPoint GrabCell = FIntPoint::ZeroValue;
    virtual void Drop_Implementation(const FPointerEvent& PointerEvent) override;
    virtual void DragCancelled_Implementation(const FPointerEvent& PointerEvent) override;
};

// A viewport drop target below the inventory/HUD. Only hit-testable during an item drag.
UCLASS()
class CHARACTER_LAB_API UARPGWorldDropTarget : public UUserWidget
{
    GENERATED_BODY()
protected:
    virtual void NativeOnInitialized() override;
    virtual bool NativeOnDrop(const FGeometry& Geometry, const FDragDropEvent& Event, UDragDropOperation* Operation) override;
};
