#pragma once
#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "ARPGAttributeSet.generated.h"

#define ARPG_ATTRIBUTE_ACCESSORS(Name) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UARPGAttributeSet, Name) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(Name) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(Name) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(Name)

UCLASS()
class CHARACTER_LAB_API UARPGAttributeSet : public UAttributeSet
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Health, Category="Vitals")
    FGameplayAttributeData Health;
    ARPG_ATTRIBUTE_ACCESSORS(Health)
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxHealth, Category="Vitals")
    FGameplayAttributeData MaxHealth;
    ARPG_ATTRIBUTE_ACCESSORS(MaxHealth)
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Mana, Category="Vitals")
    FGameplayAttributeData Mana;
    ARPG_ATTRIBUTE_ACCESSORS(Mana)
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxMana, Category="Vitals")
    FGameplayAttributeData MaxMana;
    ARPG_ATTRIBUTE_ACCESSORS(MaxMana)

    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
private:
    UFUNCTION() void OnRep_Health(const FGameplayAttributeData& Previous);
    UFUNCTION() void OnRep_MaxHealth(const FGameplayAttributeData& Previous);
    UFUNCTION() void OnRep_Mana(const FGameplayAttributeData& Previous);
    UFUNCTION() void OnRep_MaxMana(const FGameplayAttributeData& Previous);
};
#undef ARPG_ATTRIBUTE_ACCESSORS
