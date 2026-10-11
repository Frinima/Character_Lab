#include "ARPGHUDWidget.h"
#include "ARPGAttributeSet.h"
#include "ARPGPlayerState.h"
#include "ARPGPlayerController.h"
#include "Abilities/GameplayAbility.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "TimerManager.h"
#include "Engine/World.h"

void UARPGHUDWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    // TEMP: replace this minimal native UMG layout with a designer-authored Widget Blueprint.
    UBorder* Panel = WidgetTree->ConstructWidget<UBorder>();
    Panel->SetBrushColor(FLinearColor(0.015f, 0.02f, 0.03f, 0.95f));
    Panel->SetPadding(FMargin(16.f));
    UVerticalBox* Rows = WidgetTree->ConstructWidget<UVerticalBox>();
    Panel->SetContent(Rows);
    WidgetTree->RootWidget = Panel;
    auto AddRow = [Rows](UWidget* Widget) { Rows->AddChildToVerticalBox(Widget)->SetPadding(FMargin(0.f, 3.f)); };
    HealthText = WidgetTree->ConstructWidget<UTextBlock>();
    HealthBar = WidgetTree->ConstructWidget<UProgressBar>();
    HealthBar->SetFillColorAndOpacity(FLinearColor(0.75f, 0.07f, 0.06f));
    ManaText = WidgetTree->ConstructWidget<UTextBlock>();
    ManaBar = WidgetTree->ConstructWidget<UProgressBar>();
    ManaBar->SetFillColorAndOpacity(FLinearColor(0.05f, 0.35f, 0.9f));
    AddRow(HealthText); AddRow(HealthBar); AddRow(ManaText); AddRow(ManaBar);
    AbilityButton = WidgetTree->ConstructWidget<UButton>();
    AbilityText = WidgetTree->ConstructWidget<UTextBlock>();
    AbilityButton->SetContent(AbilityText);
    AbilityButton->OnClicked.AddDynamic(this, &ThisClass::ActivateAbility);
    AbilityButton->OnPressed.AddDynamic(this, &ThisClass::StopWorldMovement);
    AddRow(AbilityButton);
#if !UE_BUILD_SHIPPING
    UTextBlock* Help = WidgetTree->ConstructWidget<UTextBlock>();
    Help->SetText(NSLOCTEXT("ARPG", "DebugHUD", "TEST: H skada | J återställ"));
    AddRow(Help);
#endif
    SetVisibility(ESlateVisibility::Visible);
    RefreshVitals();
    RefreshAbility();
}
void UARPGHUDWidget::BindPlayerState(AARPGPlayerState* State)
{
    Unbind();
    ObservedState = State;
    if (State)
    {
        UAbilitySystemComponent* ASC = State->GetAbilitySystemComponent();
        for (const FGameplayAttribute& Attribute : { UARPGAttributeSet::GetHealthAttribute(), UARPGAttributeSet::GetMaxHealthAttribute(), UARPGAttributeSet::GetManaAttribute(), UARPGAttributeSet::GetMaxManaAttribute() })
            ASC->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &ThisClass::AttributeChanged);
        // Presentation timer only: remaining time is read from GAS, never simulated in UI.
        GetWorld()->GetTimerManager().SetTimer(CooldownRefreshTimer, this, &ThisClass::RefreshAbility, 0.1f, true);
    }
    RefreshVitals();
    RefreshAbility();
}
void UARPGHUDWidget::Unbind()
{
    if (AARPGPlayerState* State = ObservedState.Get())
    {
        UAbilitySystemComponent* ASC = State->GetAbilitySystemComponent();
        for (const FGameplayAttribute& Attribute : { UARPGAttributeSet::GetHealthAttribute(), UARPGAttributeSet::GetMaxHealthAttribute(), UARPGAttributeSet::GetManaAttribute(), UARPGAttributeSet::GetMaxManaAttribute() })
            ASC->GetGameplayAttributeValueChangeDelegate(Attribute).RemoveAll(this);
    }
    if (GetWorld()) GetWorld()->GetTimerManager().ClearTimer(CooldownRefreshTimer);
    ObservedState.Reset();
}
void UARPGHUDWidget::NativeDestruct() { Unbind(); Super::NativeDestruct(); }
void UARPGHUDWidget::AttributeChanged(const FOnAttributeChangeData& Data) { RefreshVitals(); RefreshAbility(); }
void UARPGHUDWidget::RefreshVitals()
{
    if (!HealthText || !ManaText) return;
    AARPGPlayerState* State = ObservedState.Get();
    UAbilitySystemComponent* ASC = State ? State->GetAbilitySystemComponent() : nullptr;
    auto Refresh = [ASC](UTextBlock* Label, UProgressBar* Bar, const FText& Name, FGameplayAttribute CurrentAttribute, FGameplayAttribute MaxAttribute)
    {
        const float Value = ASC ? ASC->GetNumericAttribute(CurrentAttribute) : 0.f;
        const float Maximum = ASC ? ASC->GetNumericAttribute(MaxAttribute) : 0.f;
        Label->SetText(FText::Format(NSLOCTEXT("ARPG", "VitalFormat", "{0}: {1} / {2}"), Name, FText::AsNumber(FMath::RoundToInt(Value)), FText::AsNumber(FMath::RoundToInt(Maximum))));
        Bar->SetPercent(Maximum > 0.f ? FMath::Clamp(Value / Maximum, 0.f, 1.f) : 0.f);
    };
    Refresh(HealthText, HealthBar, NSLOCTEXT("ARPG", "Health", "Hälsa"), UARPGAttributeSet::GetHealthAttribute(), UARPGAttributeSet::GetMaxHealthAttribute());
    Refresh(ManaText, ManaBar, NSLOCTEXT("ARPG", "Mana", "Mana"), UARPGAttributeSet::GetManaAttribute(), UARPGAttributeSet::GetMaxManaAttribute());
}
void UARPGHUDWidget::RefreshAbility()
{
    if (!AbilityText) return;
    AARPGPlayerState* State = ObservedState.Get();
    UAbilitySystemComponent* ASC = State ? State->GetAbilitySystemComponent() : nullptr;
    FGameplayAbilitySpec* Spec = ASC && State->GetPrimaryAbility() ? ASC->FindAbilitySpecFromClass(State->GetPrimaryAbility()) : nullptr;
    float Remaining = 0.f, Duration = 0.f;
    bool bReady = false;
    if (Spec && Spec->Ability && ASC->AbilityActorInfo.IsValid())
    {
        Spec->Ability->GetCooldownTimeRemainingAndDuration(Spec->Handle, ASC->AbilityActorInfo.Get(), Remaining, Duration);
        bReady = Spec->Ability->CanActivateAbility(Spec->Handle, ASC->AbilityActorInfo.Get());
    }
    // TEMP: presentation for the single prototype ability; replace with ability display data.
    if (Remaining > 0.f)
        AbilityText->SetText(FText::Format(NSLOCTEXT("ARPG", "HealCooldown", "[Q] Läkning — {0} s"), FText::AsNumber(FMath::CeilToInt(Remaining))));
    else
        AbilityText->SetText(bReady ? NSLOCTEXT("ARPG", "HealReady", "[Q] Läkning — redo") : NSLOCTEXT("ARPG", "HealUnavailable", "[Q] Läkning — ej tillgänglig"));
    // Keep the button hit-testable even when unavailable, so clicks cannot move the pawn through it.
}
void UARPGHUDWidget::ActivateAbility()
{
    if (AARPGPlayerState* State = ObservedState.Get()) State->ActivatePrimaryAbility();
    RefreshAbility();
}
void UARPGHUDWidget::StopWorldMovement()
{
    if (AARPGPlayerController* PC = Cast<AARPGPlayerController>(GetOwningPlayer())) PC->StopMovementForUI();
}
FReply UARPGHUDWidget::NativeOnMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& Event)
{
    StopWorldMovement();
    return FReply::Handled();
}
