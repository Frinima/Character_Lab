#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ARPGEquipmentPresentation.generated.h"
class UARPGInventoryComponent;
class USkeletalMeshComponent;

// Presentation observes inventory state; it never creates, removes or equips items itself.
UCLASS(ClassGroup=(ARPG), meta=(BlueprintSpawnableComponent))
class CHARACTER_LAB_API UARPGEquipmentPresentation : public UActorComponent
{
    GENERATED_BODY()
public:
    void Bind(UARPGInventoryComponent* Inventory, USkeletalMeshComponent* ChestVisual);
protected:
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
private:
    TWeakObjectPtr<UARPGInventoryComponent> Source;
    TWeakObjectPtr<USkeletalMeshComponent> ChestMesh;
    void Refresh();
};
