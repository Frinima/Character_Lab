#pragma once
#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ARPGHealAbility.generated.h"

UCLASS()
class CHARACTER_LAB_API UARPGHealAbility : public UGameplayAbility
{
    GENERATED_BODY()
public:
    UARPGHealAbility();
    virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
protected:
    UPROPERTY(EditDefaultsOnly, Category="Healing")
    TSubclassOf<UGameplayEffect> HealEffect;
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
