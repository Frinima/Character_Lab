#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ARPGInventoryWidget.generated.h"

class UARPGInventoryComponent;
class UARPGInventoryWidget;
class UARPGItemDefinition;
class UCanvasPanel;
class UHorizontalBox;
class UTextBlock;
class UImage;
class UBorder;
class USizeBox;

UCLASS()
class CHARACTER_LAB_API UARPGInventoryTile : public UUserWidget
{
    GENERATED_BODY()
public:
    void Configure(UARPGInventoryWidget* InPanel, UARPGItemDefinition* Definition, FGuid Id, bool bSelected);
protected:
    virtual void NativeOnInitialized() override;
    virtual FReply NativeOnMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& Event) override;
    virtual FReply NativeOnMouseButtonUp(const FGeometry& Geometry, const FPointerEvent& Event) override;
    virtual void NativeOnDragDetected(const FGeometry& Geometry, const FPointerEvent& Event, UDragDropOperation*& Operation) override;
private:
    TWeakObjectPtr<UARPGInventoryWidget> Panel;
    FGuid ItemId;
    FVector2D GrabFraction = FVector2D::ZeroVector;
    UPROPERTY(Transient) TObjectPtr<UImage> Icon;
    UPROPERTY(Transient) TObjectPtr<UBorder> Background;
};

UCLASS()
class CHARACTER_LAB_API UARPGInventoryWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    void BindInventory(UARPGInventoryComponent* Inventory);
    void SelectItem(FGuid Id);
    void ShowMessage(const FText& Message);
    UDragDropOperation* BeginItemDrag(FGuid Id, FVector2D GrabFraction);
protected:
    virtual void NativeOnInitialized() override;
    virtual void NativeDestruct() override;
    virtual bool NativeOnDrop(const FGeometry& Geometry, const FDragDropEvent& Event, UDragDropOperation* Operation) override;
    virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& Event) override;
    virtual FReply NativeOnMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& Event) override;
private:
    TWeakObjectPtr<UARPGInventoryComponent> Source;
    FGuid SelectedId;
    UPROPERTY(Transient) TObjectPtr<UCanvasPanel> Grid;
    UPROPERTY(Transient) TObjectPtr<UHorizontalBox> Equipment;
    UPROPERTY(Transient) TObjectPtr<UBorder> EquipmentTarget;
    UPROPERTY(Transient) TObjectPtr<UHorizontalBox> WeaponEquipment;
    UPROPERTY(Transient) TObjectPtr<UBorder> WeaponTarget;
    UPROPERTY(Transient) TObjectPtr<UTextBlock> Details;
    UPROPERTY(Transient) TObjectPtr<UTextBlock> Status;
    UPROPERTY(Transient) TObjectPtr<UTextBlock> ActionLabel;
    UPROPERTY(Transient) TObjectPtr<USizeBox> GridBox;
    void Refresh();
    void StopMovement();
    UFUNCTION() void ToggleEquipment();
    UFUNCTION() void Close();
    // TEMP: initial native UMG layout. Move style/layout to an authored Widget Blueprint.
    static constexpr float CellSize = 36.f;
};
