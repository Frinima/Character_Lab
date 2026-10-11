#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "ARPGAttributeSet.h"
#include "ARPGPrototypeEffects.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FARPGVitalBoundsTest, "ARPG.GAS.VitalBounds", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FARPGVitalBoundsTest::RunTest(const FString& Parameters)
{
    UARPGAttributeSet* Attributes = NewObject<UARPGAttributeSet>();
    Attributes->InitMaxHealth(100.f);
    Attributes->InitMaxMana(80.f);
    float Value = -25.f;
    Attributes->PreAttributeChange(UARPGAttributeSet::GetHealthAttribute(), Value);
    TestEqual(TEXT("Health cannot go below zero"), Value, 0.f);
    Value = 150.f;
    Attributes->PreAttributeChange(UARPGAttributeSet::GetHealthAttribute(), Value);
    TestEqual(TEXT("Health cannot exceed maximum"), Value, 100.f);
    Value = 100.f;
    Attributes->PreAttributeChange(UARPGAttributeSet::GetManaAttribute(), Value);
    TestEqual(TEXT("Mana cannot exceed maximum"), Value, 80.f);
    Value = -1.f;
    Attributes->PreAttributeChange(UARPGAttributeSet::GetMaxManaAttribute(), Value);
    TestEqual(TEXT("Maximum must stay positive"), Value, 1.f);
    const UARPGHealCooldownEffect* Cooldown = GetDefault<UARPGHealCooldownEffect>();
    TestTrue(TEXT("Cooldown is a timed GAS effect"), Cooldown->DurationPolicy == EGameplayEffectDurationType::HasDuration);
    TestTrue(TEXT("Cooldown grants a blocking tag"), Cooldown->GetGrantedTags().HasTagExact(FGameplayTag::RequestGameplayTag(TEXT("Cooldown.Ability.Heal"))));
    return true;
}
#endif
