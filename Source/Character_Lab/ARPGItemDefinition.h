#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Styling/SlateBrush.h"
#include "ARPGItemDefinition.generated.h"

class USkeletalMesh;
class UStaticMesh;

UENUM(BlueprintType)
enum class EARPGEquipmentSlot : uint8 { None, Chest, Weapon };

// Shared immutable definition; position, identity and equipped state belong to an item instance.
UCLASS(BlueprintType)
class CHARACTER_LAB_API UARPGItemDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item") FText DisplayName;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item", meta=(ClampMin="1")) FIntPoint Size = FIntPoint(1, 1);
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item") FSlateBrush Icon;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Equipment") EARPGEquipmentSlot EquipmentSlot = EARPGEquipmentSlot::None;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Equipment") TObjectPtr<USkeletalMesh> EquippedMesh;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Equipment|Weapon") TObjectPtr<UStaticMesh> WeaponMesh;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Equipment|Weapon") FName AttachmentSocket;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Equipment|Weapon") FTransform AttachmentTransform = FTransform::Identity;
};
