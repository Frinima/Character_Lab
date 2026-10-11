#include "ARPGDroppedItem.h"
#include "ARPGPlayerController.h"
#include "ChestLootWidget.h"
#include "Components/WidgetComponent.h"
#include "Components/SceneComponent.h"

AARPGDroppedItem::AARPGDroppedItem()
{
    PrimaryActorTick.bCanEverTick = false;
    SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("GroundAnchor")));
    Label = CreateDefaultSubobject<UWidgetComponent>(TEXT("LootLabel"));
    Label->SetupAttachment(GetRootComponent());
    Label->SetWidgetSpace(EWidgetSpace::Screen);
    // TEMP: prototype label style/offset; move to authored loot presentation settings.
    Label->SetDrawSize(FVector2D(200.f, 60.f));
    Label->SetRelativeLocation(FVector(0.f, 0.f, 90.f));
    Label->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
UChestLootWidget* AARPGDroppedItem::InitializeItem(const FARPGInventoryItem& Item, AARPGPlayerController* PC)
{
    if (!Item.Definition || !PC) return nullptr;
    StoredItem = Item;
    StoredItem.bEquipped = false;
    UChestLootWidget* Widget = CreateWidget<UChestLootWidget>(PC);
    if (!Widget) return nullptr;
    Widget->Item = this;
    Widget->Plate = this;
    Widget->Definition = Item.Definition;
    Widget->SetItemName(Item.Definition->DisplayName);
    Label->SetWidget(Widget);
    return Widget;
}
bool AARPGDroppedItem::TryCollect(UARPGInventoryComponent* Inventory, FText& Error)
{
    if (!HasAuthority() || !Inventory || bCollecting || IsHidden()) return false;
    bCollecting = true;
    if (!Inventory->TryImport(StoredItem, Error)) { bCollecting = false; return false; }
    SetActorHiddenInGame(true);
    Destroy();
    return true;
}
