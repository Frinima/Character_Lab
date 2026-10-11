#include "ARPGInventoryComponent.h"
#include "GameFramework/Actor.h"

UARPGInventoryComponent::UARPGInventoryComponent() { PrimaryComponentTick.bCanEverTick = false; }
const FARPGInventoryItem* UARPGInventoryComponent::FindItem(FGuid Id) const
{
    return Items.FindByPredicate([Id](const FARPGInventoryItem& Entry) { return Entry.Id == Id; });
}
const FARPGInventoryItem* UARPGInventoryComponent::FindEquipped(EARPGEquipmentSlot Slot) const
{
    if (Slot == EARPGEquipmentSlot::None) return nullptr;
    return Items.FindByPredicate([Slot](const FARPGInventoryItem& Entry) { return Entry.bEquipped && Entry.Definition && Entry.Definition->EquipmentSlot == Slot; });
}
bool UARPGInventoryComponent::CanPlace(FIntPoint Size, FIntPoint Position, FGuid Ignore) const
{
    if (Size.X <= 0 || Size.Y <= 0 || Position.X < 0 || Position.Y < 0 || Size.X > GridSize.X || Size.Y > GridSize.Y || Position.X > GridSize.X - Size.X || Position.Y > GridSize.Y - Size.Y) return false;
    for (const FARPGInventoryItem& Entry : Items)
    {
        if (Entry.bEquipped || Entry.Id == Ignore || !Entry.Definition) continue;
        const FIntPoint OtherEnd = Entry.Position + Entry.Definition->Size;
        if (Position.X < OtherEnd.X && Position.X + Size.X > Entry.Position.X && Position.Y < OtherEnd.Y && Position.Y + Size.Y > Entry.Position.Y) return false;
    }
    return true;
}
bool UARPGInventoryComponent::FindSpace(FIntPoint Size, FIntPoint& OutPosition, FGuid Ignore) const
{
    if (Size.X <= 0 || Size.Y <= 0 || GridSize.X < Size.X || GridSize.Y < Size.Y) return false;
    for (int32 Y = 0; Y <= GridSize.Y - Size.Y; ++Y)
        for (int32 X = 0; X <= GridSize.X - Size.X; ++X)
            if (CanPlace(Size, FIntPoint(X, Y), Ignore)) { OutPosition = FIntPoint(X, Y); return true; }
    return false;
}
bool UARPGInventoryComponent::CanModify(FText& OutError) const
{
    OutError = FText::GetEmpty();
    if (GetOwner() && GetOwner()->HasAuthority()) return true;
    OutError = NSLOCTEXT("Inventory", "Authority", "Inventory stöds ännu bara i det lokala spelet.");
    return false;
}
bool UARPGInventoryComponent::TryAdd(UARPGItemDefinition* Definition, FGuid& OutId, FText& OutError)
{
    OutId.Invalidate();
    if (!CanModify(OutError)) return false;
    if (!IsValid(Definition)) { OutError = NSLOCTEXT("Inventory", "NoDefinition", "Föremålsdata saknas."); return false; }
    FIntPoint Position;
    if (!FindSpace(Definition->Size, Position)) { OutError = NSLOCTEXT("Inventory", "Full", "Det finns ingen sammanhängande plats i inventoryt."); return false; }
    FARPGInventoryItem& Entry = Items.AddDefaulted_GetRef();
    Entry.Id = FGuid::NewGuid();
    Entry.Definition = Definition;
    Entry.Position = Position;
    OutId = Entry.Id;
    OnChanged.Broadcast();
    return true;
}
bool UARPGInventoryComponent::TryEquip(FGuid Id, FText& OutError)
{
    if (!CanModify(OutError)) return false;
    FARPGInventoryItem* Entry = Items.FindByPredicate([Id](const FARPGInventoryItem& Item) { return Item.Id == Id; });
    if (!Entry || Entry->bEquipped || !Entry->Definition)
    { OutError = NSLOCTEXT("Inventory", "NotEquipment", "Föremålet kan inte utrustas."); return false; }
    const UARPGItemDefinition* Definition = Entry->Definition;
    const bool bValidVisual =
        (Definition->EquipmentSlot == EARPGEquipmentSlot::Chest && Definition->EquippedMesh) ||
        (Definition->EquipmentSlot == EARPGEquipmentSlot::Weapon && Definition->WeaponMesh && !Definition->AttachmentSocket.IsNone());
    if (!bValidVisual)
    { OutError = NSLOCTEXT("Inventory", "NotEquipment", "Föremålet kan inte utrustas."); return false; }
    if (FindEquipped(Entry->Definition->EquipmentSlot))
    { OutError = NSLOCTEXT("Inventory", "SlotOccupied", "Ta av nuvarande utrustning först."); return false; }
    Entry->bEquipped = true;
    OnChanged.Broadcast();
    return true;
}
bool UARPGInventoryComponent::TryEquipToSlot(FGuid Id, EARPGEquipmentSlot Slot, FText& OutError)
{
    if (!CanModify(OutError)) return false;
    const FARPGInventoryItem* Entry = FindItem(Id);
    if (!Entry || !Entry->Definition || Slot == EARPGEquipmentSlot::None || Entry->Definition->EquipmentSlot != Slot)
    { OutError = NSLOCTEXT("Inventory", "WrongEquipmentSlot", "Föremålet passar inte på den utrustningsplatsen."); return false; }
    if (Entry->bEquipped) return true; // Dropping back onto its own slot is a no-op.
    return TryEquip(Id, OutError);
}
bool UARPGInventoryComponent::TryUnequip(FGuid Id, FText& OutError)
{
    if (!CanModify(OutError)) return false;
    FARPGInventoryItem* Entry = Items.FindByPredicate([Id](const FARPGInventoryItem& Item) { return Item.Id == Id; });
    if (!Entry || !Entry->bEquipped || !Entry->Definition) { OutError = NSLOCTEXT("Inventory", "NotEquipped", "Föremålet är inte utrustat."); return false; }
    FIntPoint Position;
    if (!FindSpace(Entry->Definition->Size, Position)) { OutError = NSLOCTEXT("Inventory", "UnequipFull", "Frigör plats i inventoryt innan du tar av utrustningen."); return false; }
    Entry->Position = Position;
    Entry->bEquipped = false;
    OnChanged.Broadcast();
    return true;
}
bool UARPGInventoryComponent::TryMove(FGuid Id, FIntPoint Position, FText& OutError)
{
    if (!CanModify(OutError)) return false;
    FARPGInventoryItem* Entry = Items.FindByPredicate([Id](const FARPGInventoryItem& Item) { return Item.Id == Id; });
    if (!Entry || Entry->bEquipped || !Entry->Definition || !CanPlace(Entry->Definition->Size, Position, Id))
    { OutError = NSLOCTEXT("Inventory", "CannotMove", "Föremålet får inte plats där."); return false; }
    Entry->Position = Position;
    OnChanged.Broadcast();
    return true;
}

bool UARPGInventoryComponent::TryStoreAt(FGuid Id, FIntPoint Position, FText& OutError)
{
    if (!CanModify(OutError)) return false;
    FARPGInventoryItem* Entry = Items.FindByPredicate([Id](const FARPGInventoryItem& Item) { return Item.Id == Id; });
    if (!Entry || !Entry->Definition || !CanPlace(Entry->Definition->Size, Position, Id))
    { OutError = NSLOCTEXT("Inventory", "CannotMove", "Föremålet får inte plats där."); return false; }
    Entry->Position = Position;
    Entry->bEquipped = false;
    OnChanged.Broadcast();
    return true;
}
bool UARPGInventoryComponent::TryImport(const FARPGInventoryItem& Item, FText& OutError)
{
    if (!CanModify(OutError)) return false;
    FIntPoint Position;
    if (!Item.Id.IsValid() || !IsValid(Item.Definition) || FindItem(Item.Id) || !FindSpace(Item.Definition->Size, Position))
    { OutError = NSLOCTEXT("Inventory", "ImportFailed", "Föremålet kunde inte tas emot. Kontrollera ledig plats."); return false; }
    FARPGInventoryItem Copy = Item;
    Copy.Position = Position;
    Copy.bEquipped = false;
    Items.Add(Copy);
    OnChanged.Broadcast();
    return true;
}
bool UARPGInventoryComponent::TryRemove(FGuid Id, FText& OutError)
{
    if (!CanModify(OutError)) return false;
    const int32 Index = Items.IndexOfByPredicate([Id](const FARPGInventoryItem& Item) { return Item.Id == Id; });
    if (Index == INDEX_NONE) { OutError = NSLOCTEXT("Inventory", "MissingItem", "Föremålet finns inte kvar."); return false; }
    Items.RemoveAt(Index);
    OnChanged.Broadcast();
    return true;
}
