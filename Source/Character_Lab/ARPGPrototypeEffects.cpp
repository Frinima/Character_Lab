#include "ARPGPrototypeEffects.h"
#include "ARPGAttributeSet.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"
#include "NativeGameplayTags.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_HealCooldown, "Cooldown.Ability.Heal");

namespace
{
    void AddModifier(UGameplayEffect& Effect, FGameplayAttribute Attribute, EGameplayModOp::Type Operation, float Value)
    {
        FGameplayModifierInfo& Modifier = Effect.Modifiers.AddDefaulted_GetRef();
        Modifier.Attribute = Attribute;
        Modifier.ModifierOp = Operation;
        Modifier.ModifierMagnitude = FScalableFloat(Value);
    }
}

UARPGInitialVitalsEffect::UARPGInitialVitalsEffect()
{
    DurationPolicy = EGameplayEffectDurationType::Instant;
    AddModifier(*this, UARPGAttributeSet::GetMaxHealthAttribute(), EGameplayModOp::Override, 100.f);
    AddModifier(*this, UARPGAttributeSet::GetMaxManaAttribute(), EGameplayModOp::Override, 100.f);
    AddModifier(*this, UARPGAttributeSet::GetHealthAttribute(), EGameplayModOp::Override, 100.f);
    AddModifier(*this, UARPGAttributeSet::GetManaAttribute(), EGameplayModOp::Override, 100.f);
}
UARPGHealEffect::UARPGHealEffect()
{
    DurationPolicy = EGameplayEffectDurationType::Instant;
    AddModifier(*this, UARPGAttributeSet::GetHealthAttribute(), EGameplayModOp::Additive, 30.f);
}
UARPGHealCostEffect::UARPGHealCostEffect()
{
    DurationPolicy = EGameplayEffectDurationType::Instant;
    AddModifier(*this, UARPGAttributeSet::GetManaAttribute(), EGameplayModOp::Additive, -20.f);
}
UARPGHealCooldownEffect::UARPGHealCooldownEffect()
{
    DurationPolicy = EGameplayEffectDurationType::HasDuration;
    DurationMagnitude = FScalableFloat(3.f);
    FInheritedTagContainer Tags;
    Tags.AddTag(TAG_HealCooldown);
    // Standard UE Gameplay Effect component, not a parallel cooldown timer.
    UTargetTagsGameplayEffectComponent* TagComponent = CreateDefaultSubobject<UTargetTagsGameplayEffectComponent>(TEXT("CooldownTags"));
    GEComponents.Add(TagComponent);
    TagComponent->SetAndApplyTargetTagChanges(Tags);
}
UARPGDebugDamageEffect::UARPGDebugDamageEffect()
{
    DurationPolicy = EGameplayEffectDurationType::Instant;
    AddModifier(*this, UARPGAttributeSet::GetHealthAttribute(), EGameplayModOp::Additive, -25.f);
}
