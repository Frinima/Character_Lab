#include "ARPGEquipmentPresentation.h"
#include "ARPGInventoryComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"

void UARPGEquipmentPresentation::Bind(UARPGInventoryComponent* Inventory, USkeletalMeshComponent* ChestVisual, USkeletalMeshComponent* BodyVisual)
{
    if (Source.IsValid()) Source->OnChanged.RemoveAll(this);
    Source = Inventory;
    ChestMesh = ChestVisual;
    BodyMesh = BodyVisual;
    if (Source.IsValid()) Source->OnChanged.AddUObject(this, &ThisClass::Refresh);
    Refresh();
}
void UARPGEquipmentPresentation::Refresh()
{
    const FARPGInventoryItem* Entry = Source.IsValid() ? Source->FindEquipped(EARPGEquipmentSlot::Chest) : nullptr;
    if (ChestMesh.IsValid())
    {
        if (Entry) ChestMesh->SetSkeletalMeshAsset(Entry->Definition->EquippedMesh);
        ChestMesh->SetVisibility(Entry != nullptr);
    }
    const FARPGInventoryItem* Weapon = Source.IsValid() ? Source->FindEquipped(EARPGEquipmentSlot::Weapon) : nullptr;
    if (WeaponVisual)
    {
        WeaponVisual->SetVisibility(false);
        WeaponVisual->SetStaticMesh(nullptr);
    }
    if (!Weapon || !BodyMesh.IsValid()) return;
    const UARPGItemDefinition* Definition = Weapon->Definition;
    if (!BodyMesh->DoesSocketExist(Definition->AttachmentSocket))
    {
        UE_LOG(LogTemp, Error, TEXT("Weapon %s: attachment socket '%s' is missing on %s."),
            *GetNameSafe(Definition), *Definition->AttachmentSocket.ToString(), *GetNameSafe(BodyMesh.Get()));
        return;
    }
    if (!WeaponVisual)
    {
        WeaponVisual = NewObject<UStaticMeshComponent>(GetOwner(), TEXT("EquippedWeapon"));
        GetOwner()->AddInstanceComponent(WeaponVisual);
        WeaponVisual->SetMobility(EComponentMobility::Movable);
        WeaponVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        WeaponVisual->SetGenerateOverlapEvents(false);
        WeaponVisual->SetCanEverAffectNavigation(false);
        WeaponVisual->RegisterComponent();
    }
    WeaponVisual->AttachToComponent(BodyMesh.Get(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, Definition->AttachmentSocket);
    WeaponVisual->SetRelativeTransform(Definition->AttachmentTransform);
    WeaponVisual->SetStaticMesh(Definition->WeaponMesh);
    WeaponVisual->SetVisibility(true);
}
void UARPGEquipmentPresentation::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (Source.IsValid()) Source->OnChanged.RemoveAll(this);
    if (WeaponVisual) WeaponVisual->DestroyComponent();
    WeaponVisual = nullptr;
    Super::EndPlay(EndPlayReason);
}
