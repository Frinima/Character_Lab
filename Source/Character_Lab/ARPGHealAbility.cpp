#include "ARPGHealAbility.h"
#include "ARPGAttributeSet.h"
#include "ARPGPrototypeEffects.h"

UARPGHealAbility::UARPGHealAbility()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
    // TEMP: native test assets; replace defaults with authored Gameplay Effects.
    HealEffect = UARPGHealEffect::StaticClass();
    CostGameplayEffectClass = UARPGHealCostEffect::StaticClass();
    CooldownGameplayEffectClass = UARPGHealCooldownEffect::StaticClass();
}
bool UARPGHealAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
    if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags) || !HealEffect || !ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid()) return false;
    const UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
    const float Health = ASC->GetNumericAttribute(UARPGAttributeSet::GetHealthAttribute());
    return Health > 0.f && Health < ASC->GetNumericAttribute(UARPGAttributeSet::GetMaxHealthAttribute());
}
void UARPGHealAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }
    ApplyGameplayEffectToOwner(Handle, ActorInfo, ActivationInfo, HealEffect.GetDefaultObject(), GetAbilityLevel(Handle, ActorInfo));
    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
