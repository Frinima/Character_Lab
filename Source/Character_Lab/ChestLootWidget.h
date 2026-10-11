#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChestLootWidget.generated.h"

class UARPGItemDefinition;

// Input wrapper: keeps the existing Blueprint label's appearance intact.
UCLASS()
class CHARACTER_LAB_API UChestLootWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    void SetLabel(UUserWidget* Label);
    void SetItemName(const FText& Name);
    TWeakObjectPtr<AActor> Item;
    TWeakObjectPtr<AActor> Plate;
    TWeakObjectPtr<AActor> Stem;
    UPROPERTY(Transient) TObjectPtr<UARPGItemDefinition> Definition;
    bool IsAvailable() const;
    bool TryPickup(APawn* Recipient);
protected:
    virtual FReply NativeOnMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& Event) override;
};
