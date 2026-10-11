#include "ARPGPlayerState.h"
#include "AbilitySystemComponent.h"
#include "ARPGAttributeSet.h"
#include "ARPGHealAbility.h"
#include "ARPGPrototypeEffects.h"
#include "GameFramework/Pawn.h"
#include "ARPGInventoryComponent.h"

AARPGPlayerState::AARPGPlayerState()
{
    Inventory = CreateDefaultSubobject<UARPGInventoryComponent>(TEXT("Inventory"));
    AbilitySystem = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));
    AbilitySystem->SetIsReplicated(true);
    AbilitySystem->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
    Attributes = CreateDefaultSubobject<UARPGAttributeSet>(TEXT("Attributes"));
    // TEMP: replace these prototype defaults with authored Blueprint classes.
    InitialVitals = UARPGInitialVitalsEffect::StaticClass();
    PrimaryAbility = UARPGHealAbility::StaticClass();
}
UAbilitySystemComponent* AARPGPlayerState::GetAbilitySystemComponent() const { return AbilitySystem; }

void AARPGPlayerState::InitializeAbilitySystem(APawn* Avatar)
{
    if (!Avatar) return;
    AbilitySystem->InitAbilityActorInfo(this, Avatar);
    if (HasAuthority() && !bStartupGranted)
    {
        ApplyEffect(InitialVitals);
        if (PrimaryAbility) AbilitySystem->GiveAbility(FGameplayAbilitySpec(PrimaryAbility, 1));
        bStartupGranted = true;
    }
}
void AARPGPlayerState::ApplyEffect(TSubclassOf<UGameplayEffect> Effect)
{
    if (!HasAuthority() || !Effect) return;
    FGameplayEffectContextHandle Context = AbilitySystem->MakeEffectContext();
    Context.AddSourceObject(this);
    FGameplayEffectSpecHandle Spec = AbilitySystem->MakeOutgoingSpec(Effect, 1.f, Context);
    if (Spec.IsValid()) AbilitySystem->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
}
void AARPGPlayerState::ActivatePrimaryAbility()
{
    if (PrimaryAbility) AbilitySystem->TryActivateAbilityByClass(PrimaryAbility);
}
void AARPGPlayerState::DebugDamage()
{
#if !UE_BUILD_SHIPPING
    ApplyEffect(UARPGDebugDamageEffect::StaticClass());
#endif
}
void AARPGPlayerState::DebugRestore()
{
#if !UE_BUILD_SHIPPING
    ApplyEffect(InitialVitals);
#endif
}
