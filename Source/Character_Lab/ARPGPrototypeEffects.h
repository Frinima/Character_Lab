#pragma once
#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "ARPGPrototypeEffects.generated.h"

// TEMP: test balancing defaults. Replace with designer-authored Gameplay Effect assets.
UCLASS() class CHARACTER_LAB_API UARPGInitialVitalsEffect : public UGameplayEffect
{
    GENERATED_BODY()
public: UARPGInitialVitalsEffect();
};
UCLASS() class CHARACTER_LAB_API UARPGHealEffect : public UGameplayEffect
{
    GENERATED_BODY()
public: UARPGHealEffect();
};
UCLASS() class CHARACTER_LAB_API UARPGHealCostEffect : public UGameplayEffect
{
    GENERATED_BODY()
public: UARPGHealCostEffect();
};
UCLASS() class CHARACTER_LAB_API UARPGHealCooldownEffect : public UGameplayEffect
{
    GENERATED_BODY()
public: UARPGHealCooldownEffect();
};
UCLASS() class CHARACTER_LAB_API UARPGDebugDamageEffect : public UGameplayEffect
{
    GENERATED_BODY()
public: UARPGDebugDamageEffect();
};
