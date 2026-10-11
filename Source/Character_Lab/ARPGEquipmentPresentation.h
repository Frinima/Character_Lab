#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ARPGEquipmentPresentation.generated.h"
class UARPGInventoryComponent;
class USkeletalMeshComponent;
class UStaticMeshComponent;

// Presentation observes inventory state; it never creates, removes or equips items itself.
UCLASS(ClassGroup=(ARPG), meta=(BlueprintSpawnableComponent))
class CHARACTER_LAB_API UARPGEquipmentPresentation : public UActorComponent
{
    GENERATED_BODY()
public:
    void Bind(UARPGInventoryComponent* Inventory, USkeletalMeshComponent* ChestVisual, USkeletalMeshComponent* BodyVisual);
protected:
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
private:
    TWeakObjectPtr<UARPGInventoryComponent> Source;
    TWeakObjectPtr<USkeletalMeshComponent> ChestMesh;
    TWeakObjectPtr<USkeletalMeshComponent> BodyMesh;
    UPROPERTY(Transient) TObjectPtr<UStaticMeshComponent> WeaponVisual;
    void Refresh();
};
