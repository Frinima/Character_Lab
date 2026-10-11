#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ARPGPlayerController.generated.h"

class UChestLootWidget;
class UARPGHUDWidget;
class UARPGInventoryWidget;
class UARPGItemDefinition;
class UARPGWorldDropTarget;
class UARPGInventoryComponent;

UCLASS(Config=Game)
class CHARACTER_LAB_API AARPGPlayerController : public APlayerController
{
    GENERATED_BODY()
public:
    AARPGPlayerController();
    void RequestPickup(UChestLootWidget* Loot);
    void StopMovementForUI();
    void ShowInventoryMessage(const FText& Message);
    float GetPickupDistance() const { return PickupDistance; }
    void SetInventoryDragActive(bool bActive);
    void DropInventoryItem(FGuid ItemId, UARPGInventoryComponent* Source);
    virtual void AcknowledgePossession(APawn* InPawn) override;
protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;
    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnUnPossess() override;
    virtual void OnRep_PlayerState() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void PlayerTick(float DeltaSeconds) override;

    UPROPERTY(EditDefaultsOnly, Category="ARPG|Camera")
    float CameraDistance = 1100.f;
    UPROPERTY(EditDefaultsOnly, Category="ARPG|Camera")
    FRotator CameraAngle = FRotator(-60.f, -45.f, 0.f);
    UPROPERTY(EditDefaultsOnly, Category="ARPG|Movement")
    float DestinationUpdateInterval = 0.15f;
    UPROPERTY(EditDefaultsOnly, Category="ARPG|Interaction", meta=(ClampMin="100"))
    float PickupDistance = 300.f;
    UPROPERTY(EditDefaultsOnly, Category="ARPG|UI")
    TSubclassOf<UARPGHUDWidget> HUDWidgetClass;
    UPROPERTY(EditDefaultsOnly, Category="ARPG|UI")
    TSubclassOf<UARPGInventoryWidget> InventoryWidgetClass;
    UPROPERTY(EditDefaultsOnly, Config, Category="ARPG|Items")
    TSoftObjectPtr<UARPGItemDefinition> ChestItemDefinition;
    UPROPERTY(EditDefaultsOnly, Config, Category="ARPG|Testing")
    TArray<TSoftObjectPtr<UARPGItemDefinition>> InventoryTestDefinitions;
private:
    UPROPERTY(Transient) TObjectPtr<UARPGWorldDropTarget> WorldDropTarget;
    UPROPERTY(Transient) TObjectPtr<UARPGInventoryWidget> InventoryWindow;
    TArray<TWeakObjectPtr<UChestLootWidget>> LootLabels;
    void ToggleInventory();
    void InitializeEquipmentPresentation();
    UPROPERTY(Transient) TObjectPtr<UARPGHUDWidget> PlayerHUD;
    void InitializePlayerSystems();
    void ActivatePrimaryAbility();
    void DebugDamage();
    void DebugRestore();
    void AddInventoryTestItems();
    void InitializeLootLabels();
    TWeakObjectPtr<UChestLootWidget> PendingPickup;
    float PickupElapsed = 0.f;
    void BeginMove();
    void EndMove();
    void MoveToCursor();
    void PickupNearbyItem();
    bool bMoveHeld = false;
    float TimeSinceDestinationUpdate = 0.f;
};
