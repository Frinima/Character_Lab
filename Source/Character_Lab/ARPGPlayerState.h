#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "ARPGPlayerState.generated.h"

class UAbilitySystemComponent;
class UARPGAttributeSet;
class UGameplayAbility;
class UGameplayEffect;
class UARPGInventoryComponent;

UCLASS()
class CHARACTER_LAB_API AARPGPlayerState : public APlayerState, public IAbilitySystemInterface
{
    GENERATED_BODY()
public:
    AARPGPlayerState();
    UARPGInventoryComponent* GetInventory() const { return Inventory; }
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    void InitializeAbilitySystem(APawn* Avatar);
    UFUNCTION(BlueprintCallable, Category="Abilities") void ActivatePrimaryAbility();
    TSubclassOf<UGameplayAbility> GetPrimaryAbility() const { return PrimaryAbility; }
    // TEMP: development-only test controls; no debug effects in Shipping builds.
    void DebugDamage();
    void DebugRestore();
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
    TObjectPtr<UARPGInventoryComponent> Inventory;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Abilities")
    TObjectPtr<UAbilitySystemComponent> AbilitySystem;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Abilities")
    TObjectPtr<UARPGAttributeSet> Attributes;
    UPROPERTY(EditDefaultsOnly, Category="Abilities")
    TSubclassOf<UGameplayEffect> InitialVitals;
    UPROPERTY(EditDefaultsOnly, Category="Abilities")
    TSubclassOf<UGameplayAbility> PrimaryAbility;
private:
    bool bStartupGranted = false;
    void ApplyEffect(TSubclassOf<UGameplayEffect> Effect);
};
