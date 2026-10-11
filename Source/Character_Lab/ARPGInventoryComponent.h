#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ARPGItemDefinition.h"
#include "ARPGInventoryComponent.generated.h"

USTRUCT(BlueprintType)
struct FARPGInventoryItem
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadOnly) FGuid Id;
    UPROPERTY(BlueprintReadOnly) TObjectPtr<UARPGItemDefinition> Definition;
    UPROPERTY(BlueprintReadOnly) FIntPoint Position = FIntPoint::ZeroValue;
    UPROPERTY(BlueprintReadOnly) bool bEquipped = false;
};

DECLARE_MULTICAST_DELEGATE(FARPGInventoryChanged);

UCLASS(ClassGroup=(ARPG), meta=(BlueprintSpawnableComponent))
class CHARACTER_LAB_API UARPGInventoryComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UARPGInventoryComponent();
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory", meta=(ClampMin="1", ClampMax="20"))
    FIntPoint GridSize = FIntPoint(10, 6);
    const TArray<FARPGInventoryItem>& GetItems() const { return Items; }
    const FARPGInventoryItem* FindItem(FGuid Id) const;
    const FARPGInventoryItem* FindEquipped(EARPGEquipmentSlot Slot) const;
    bool FindSpace(FIntPoint Size, FIntPoint& OutPosition, FGuid Ignore = FGuid()) const;
    bool CanPlace(FIntPoint Size, FIntPoint Position, FGuid Ignore = FGuid()) const;
    bool TryAdd(UARPGItemDefinition* Definition, FGuid& OutId, FText& OutError);
    bool TryEquip(FGuid Id, FText& OutError);
    bool TryUnequip(FGuid Id, FText& OutError);
    bool TryMove(FGuid Id, FIntPoint Position, FText& OutError);
    bool TryStoreAt(FGuid Id, FIntPoint Position, FText& OutError);
    bool TryImport(const FARPGInventoryItem& Item, FText& OutError);
    bool TryRemove(FGuid Id, FText& OutError);
    FARPGInventoryChanged OnChanged;
private:
    UPROPERTY(VisibleInstanceOnly, Category="Inventory") TArray<FARPGInventoryItem> Items;
    bool CanModify(FText& OutError) const;
};
