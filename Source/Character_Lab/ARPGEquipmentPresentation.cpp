#include "ARPGEquipmentPresentation.h"
#include "ARPGInventoryComponent.h"
#include "Components/SkeletalMeshComponent.h"

void UARPGEquipmentPresentation::Bind(UARPGInventoryComponent* Inventory, USkeletalMeshComponent* ChestVisual)
{
    if (Source.IsValid()) Source->OnChanged.RemoveAll(this);
    Source = Inventory;
    ChestMesh = ChestVisual;
    if (Source.IsValid()) Source->OnChanged.AddUObject(this, &ThisClass::Refresh);
    Refresh();
}
void UARPGEquipmentPresentation::Refresh()
{
    if (!ChestMesh.IsValid()) return;
    const FARPGInventoryItem* Entry = Source.IsValid() ? Source->FindEquipped(EARPGEquipmentSlot::Chest) : nullptr;
    if (Entry)
    {
        ChestMesh->SetSkeletalMeshAsset(Entry->Definition->EquippedMesh);
        ChestMesh->SetVisibility(true);
    }
    else ChestMesh->SetVisibility(false);
}
void UARPGEquipmentPresentation::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (Source.IsValid()) Source->OnChanged.RemoveAll(this);
    Super::EndPlay(EndPlayReason);
}
