#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ARPGInventoryComponent.h"
#include "ARPGDroppedItem.generated.h"
class UWidgetComponent;
class UChestLootWidget;
class AARPGPlayerController;

UCLASS()
class CHARACTER_LAB_API AARPGDroppedItem : public AActor
{
    GENERATED_BODY()
public:
    AARPGDroppedItem();
    UChestLootWidget* InitializeItem(const FARPGInventoryItem& Item, AARPGPlayerController* PC);
    bool TryCollect(UARPGInventoryComponent* Inventory, FText& Error);
private:
    UPROPERTY(VisibleAnywhere) TObjectPtr<UWidgetComponent> Label;
    UPROPERTY(Transient) FARPGInventoryItem StoredItem;
    bool bCollecting = false;
};
