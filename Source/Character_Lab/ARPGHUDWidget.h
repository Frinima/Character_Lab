#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ARPGHUDWidget.generated.h"

class AARPGPlayerState;
class UProgressBar;
class UTextBlock;
class UButton;
struct FOnAttributeChangeData;

UCLASS()
class CHARACTER_LAB_API UARPGHUDWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    void BindPlayerState(AARPGPlayerState* State);
protected:
    virtual void NativeOnInitialized() override;
    virtual void NativeDestruct() override;
    virtual FReply NativeOnMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& Event) override;
private:
    UPROPERTY(Transient) TObjectPtr<UProgressBar> HealthBar;
    UPROPERTY(Transient) TObjectPtr<UProgressBar> ManaBar;
    UPROPERTY(Transient) TObjectPtr<UTextBlock> HealthText;
    UPROPERTY(Transient) TObjectPtr<UTextBlock> ManaText;
    UPROPERTY(Transient) TObjectPtr<UTextBlock> AbilityText;
    UPROPERTY(Transient) TObjectPtr<UButton> AbilityButton;
    TWeakObjectPtr<AARPGPlayerState> ObservedState;
    FTimerHandle CooldownRefreshTimer;
    void Unbind();
    void RefreshVitals();
    void RefreshAbility();
    void AttributeChanged(const FOnAttributeChangeData& Data);
    UFUNCTION() void ActivateAbility();
    UFUNCTION() void StopWorldMovement();
};
