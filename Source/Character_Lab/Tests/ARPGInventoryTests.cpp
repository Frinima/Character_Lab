#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "ARPGInventoryComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Engine/SkeletalMesh.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FARPGInventoryGridTest, "ARPG.Inventory.GridAndEquipment", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FARPGInventoryGridTest::RunTest(const FString& Parameters)
{
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
    if (!TestNotNull(TEXT("Test world"), World)) return false;
    ON_SCOPE_EXIT { World->DestroyWorld(false); };
    AActor* Owner = World->SpawnActor<AActor>();
    if (!TestNotNull(TEXT("Inventory owner"), Owner)) return false;
    UARPGInventoryComponent* Inventory = NewObject<UARPGInventoryComponent>(Owner);
    Inventory->GridSize = FIntPoint(3, 3);
    UARPGItemDefinition* Chest = NewObject<UARPGItemDefinition>();
    Chest->Size = FIntPoint(2, 3);
    Chest->EquipmentSlot = EARPGEquipmentSlot::Chest;
    Chest->EquippedMesh = NewObject<USkeletalMesh>(); // Fixture only; no rendering in model tests.
    UARPGItemDefinition* Small = NewObject<UARPGItemDefinition>();
    Small->Size = FIntPoint(1, 1);
    UARPGItemDefinition* Tall = NewObject<UARPGItemDefinition>();
    Tall->Size = FIntPoint(1, 2);
    FGuid ChestId, SmallId, TallId, RejectedId;
    FText Error;
    TestTrue(TEXT("Add 2x3 item"), Inventory->TryAdd(Chest, ChestId, Error));
    TestTrue(TEXT("Add 1x1 item"), Inventory->TryAdd(Small, SmallId, Error));
    TestTrue(TEXT("Add 1x2 item"), Inventory->TryAdd(Tall, TallId, Error));
    if (!Inventory->FindItem(ChestId) || !Inventory->FindItem(SmallId) || !Inventory->FindItem(TallId)) return false;
    TestTrue(TEXT("Small item fills remaining column"), Inventory->FindItem(SmallId)->Position == FIntPoint(2, 0));
    TestTrue(TEXT("Tall item placed below small"), Inventory->FindItem(TallId)->Position == FIntPoint(2, 1));
    TestFalse(TEXT("Full inventory rejects pickup"), Inventory->TryAdd(Small, RejectedId, Error));
    TestFalse(TEXT("Failed pickup does not return an item ID"), RejectedId.IsValid());
    TestEqual(TEXT("Failed pickup preserves item count"), Inventory->GetItems().Num(), 3);
    TestFalse(TEXT("Non-equipment cannot equip"), Inventory->TryEquip(SmallId, Error));
    TestFalse(TEXT("Overlap rejected"), Inventory->TryMove(TallId, FIntPoint(0, 0), Error));
    TestFalse(TEXT("Negative position rejected"), Inventory->CanPlace(FIntPoint(1, 1), FIntPoint(-1, 0)));
    TestFalse(TEXT("Oversized item rejected"), Inventory->CanPlace(FIntPoint(4, 1), FIntPoint(0, 0)));
    TestFalse(TEXT("Zero-size definition rejected"), Inventory->CanPlace(FIntPoint(0, 1), FIntPoint(0, 0)));
    TestTrue(TEXT("Equip owned chest"), Inventory->TryEquip(ChestId, Error));
    TestFalse(TEXT("Duplicate equip rejected"), Inventory->TryEquip(ChestId, Error));
    TestTrue(TEXT("Equipped item frees its cells"), Inventory->CanPlace(FIntPoint(2, 3), FIntPoint(0, 0)));
    TestTrue(TEXT("Unequip returns same item"), Inventory->TryUnequip(ChestId, Error));
    TestEqual(TEXT("Equip/unequip does not duplicate"), Inventory->GetItems().Num(), 3);
    TestTrue(TEXT("Equip again"), Inventory->TryEquip(ChestId, Error));
    FGuid SecondChestId;
    TestTrue(TEXT("Second instance of same definition"), Inventory->TryAdd(Chest, SecondChestId, Error));
    TestTrue(TEXT("Instances have distinct identities"), SecondChestId != ChestId);
    TestFalse(TEXT("Occupied equipment slot rejects second chest"), Inventory->TryEquip(SecondChestId, Error));
    TestFalse(TEXT("Cannot unequip into full bag"), Inventory->TryUnequip(ChestId, Error));
    TestTrue(TEXT("Failed unequip preserves equipped state"), Inventory->FindItem(ChestId)->bEquipped);
    TestFalse(TEXT("Invalid ID rejected"), Inventory->TryEquip(FGuid::NewGuid(), Error));
    TestFalse(TEXT("Drag from equipment into occupied cells fails"), Inventory->TryStoreAt(ChestId, FIntPoint(0, 0), Error));
    TestTrue(TEXT("Invalid drag preserves equipment"), Inventory->FindItem(ChestId)->bEquipped);
    const FARPGInventoryItem Dropped = *Inventory->FindItem(SecondChestId);
    TestTrue(TEXT("Remove item for world drop"), Inventory->TryRemove(SecondChestId, Error));
    TestFalse(TEXT("Same item cannot be removed twice"), Inventory->TryRemove(SecondChestId, Error));
    TestTrue(TEXT("Drag equipped item into exact free cells"), Inventory->TryStoreAt(ChestId, FIntPoint(0, 0), Error));
    TestFalse(TEXT("Successful store removes equipped state"), Inventory->FindItem(ChestId)->bEquipped);
    TestFalse(TEXT("World pickup rejected when full"), Inventory->TryImport(Dropped, Error));
    TestTrue(TEXT("Free cells by equipping"), Inventory->TryEquip(ChestId, Error));
    TestTrue(TEXT("Recollect world item"), Inventory->TryImport(Dropped, Error));
    TestTrue(TEXT("Recollected item preserves identity"), Inventory->FindItem(SecondChestId) != nullptr);
    TestFalse(TEXT("Duplicate world pickup rejected"), Inventory->TryImport(Dropped, Error));
    return true;
}
#endif
