#include "ARPGPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputCoreTypes.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "ChestLootWidget.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ARPGPlayerState.h"
#include "ARPGHUDWidget.h"
#include "AbilitySystemComponent.h"
#include "ARPGInventoryComponent.h"
#include "ARPGInventoryWidget.h"
#include "ARPGEquipmentPresentation.h"
#include "Components/SkeletalMeshComponent.h"
#include "UObject/UnrealType.h"
#include "ARPGInventoryDrag.h"
#include "ARPGDroppedItem.h"
#include "GameFramework/CharacterMovementComponent.h"

AARPGPlayerController::AARPGPlayerController()
{
    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Default;
    HUDWidgetClass = UARPGHUDWidget::StaticClass();
    InventoryWidgetClass = UARPGInventoryWidget::StaticClass();
}

void AARPGPlayerController::BeginPlay()
{
    Super::BeginPlay();
    FInputModeGameAndUI Mode;
    Mode.SetHideCursorDuringCapture(false);
    Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(Mode);
    InitializeLootLabels();
    InitializePlayerSystems();
}

void AARPGPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &ThisClass::BeginMove);
    InputComponent->BindKey(EKeys::LeftMouseButton, IE_Released, this, &ThisClass::EndMove);
    InputComponent->BindKey(EKeys::One, IE_Pressed, this, &ThisClass::PickupNearbyItem);
    // TEMP: prototype key bindings; migrate to authored Enhanced Input actions.
    InputComponent->BindKey(EKeys::Q, IE_Pressed, this, &ThisClass::ActivatePrimaryAbility);
    InputComponent->BindKey(EKeys::I, IE_Pressed, this, &ThisClass::ToggleInventory);
#if !UE_BUILD_SHIPPING
    InputComponent->BindKey(EKeys::H, IE_Pressed, this, &ThisClass::DebugDamage);
    InputComponent->BindKey(EKeys::J, IE_Pressed, this, &ThisClass::DebugRestore);
    InputComponent->BindKey(EKeys::K, IE_Pressed, this, &ThisClass::AddInventoryTestItems);
#endif
}

void AARPGPlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    InitializePlayerSystems();
    if (ACharacter* ControlledCharacter = Cast<ACharacter>(InPawn))
    {
        ControlledCharacter->bUseControllerRotationYaw = false;
        ControlledCharacter->bUseControllerRotationPitch = false;
        ControlledCharacter->bUseControllerRotationRoll = false;
        ControlledCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
        ControlledCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = false;
        ControlledCharacter->GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
        if (USpringArmComponent* Arm = ControlledCharacter->FindComponentByClass<USpringArmComponent>())
        {
            Arm->bUsePawnControlRotation = false;
            Arm->SetUsingAbsoluteRotation(true);
            Arm->SetWorldRotation(CameraAngle);
            Arm->TargetArmLength = CameraDistance;
            Arm->SocketOffset = FVector::ZeroVector;
            Arm->TargetOffset = FVector(0.f, 0.f, 40.f);
            Arm->bDoCollisionTest = false;
        }
        if (UCameraComponent* Camera = ControlledCharacter->FindComponentByClass<UCameraComponent>())
        {
            Camera->bUsePawnControlRotation = false;
            Camera->SetRelativeRotation(FRotator::ZeroRotator);
            Camera->SetFieldOfView(55.f);
        }
    }
}

void AARPGPlayerController::InitializePlayerSystems()
{
    AARPGPlayerState* State = GetPlayerState<AARPGPlayerState>();
    if (State && GetPawn()) State->InitializeAbilitySystem(GetPawn());
    InitializeEquipmentPresentation();
    if (!IsLocalController() || !HasActorBegunPlay()) return;
    if (!PlayerHUD && HUDWidgetClass)
    {
        PlayerHUD = CreateWidget<UARPGHUDWidget>(this, HUDWidgetClass);
        if (PlayerHUD)
        {
            PlayerHUD->AddToPlayerScreen(10);
            // TEMP: prototype layout. Move anchors/style to the authored HUD Widget Blueprint.
            PlayerHUD->SetDesiredSizeInViewport(FVector2D(380.f, 220.f));
            PlayerHUD->SetPositionInViewport(FVector2D(0.f, -24.f), false);
            // Size/position setters reset anchors to (0, 0). Apply the bottom-center anchor last.
            PlayerHUD->SetAnchorsInViewport(FAnchors(0.5f, 1.f));
            PlayerHUD->SetAlignmentInViewport(FVector2D(0.5f, 1.f));
        }
    }
    if (PlayerHUD) PlayerHUD->BindPlayerState(State);
    if (!InventoryWindow && InventoryWidgetClass)
    {
        InventoryWindow = CreateWidget<UARPGInventoryWidget>(this, InventoryWidgetClass);
        if (InventoryWindow)
        {
            InventoryWindow->AddToPlayerScreen(20);
            // TEMP: prototype panel placement, to be authored with the final UI layout.
            // Both dimensions zero request UMG desired-size layout; a single zero fixes that dimension at zero.
            InventoryWindow->SetDesiredSizeInViewport(FVector2D::ZeroVector);
            InventoryWindow->SetPositionInViewport(FVector2D(-20.f, 20.f), false);
            InventoryWindow->SetAnchorsInViewport(FAnchors(1.f, 0.f));
            InventoryWindow->SetAlignmentInViewport(FVector2D(1.f, 0.f));
            InventoryWindow->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
    if (InventoryWindow) InventoryWindow->BindInventory(State ? State->GetInventory() : nullptr);
}

void AARPGPlayerController::InitializeEquipmentPresentation()
{
    AARPGPlayerState* State = GetPlayerState<AARPGPlayerState>();
    APawn* ControlledPawn = GetPawn();
    if (!State || !ControlledPawn) return;
    // TEMP: explicit adapter for the existing Blueprint chest visual. Replace with an authored
    // EquipmentPresentation component and slot references when migrating the character to C++.
    FObjectProperty* MeshProperty = FindFProperty<FObjectProperty>(ControlledPawn->GetClass(), TEXT("ChestArmorMesh"));
    USkeletalMeshComponent* Visual = MeshProperty ? Cast<USkeletalMeshComponent>(MeshProperty->GetObjectPropertyValue_InContainer(ControlledPawn)) : nullptr;
    if (!Visual)
    {
        UE_LOG(LogTemp, Error, TEXT("Inventory: character has no ChestArmorMesh equipment visual."));
    }
    UARPGEquipmentPresentation* Presenter = ControlledPawn->FindComponentByClass<UARPGEquipmentPresentation>();
    if (!Presenter)
    {
        Presenter = NewObject<UARPGEquipmentPresentation>(ControlledPawn);
        ControlledPawn->AddInstanceComponent(Presenter);
        Presenter->RegisterComponent();
    }
    ACharacter* ControlledCharacter = Cast<ACharacter>(ControlledPawn);
    Presenter->Bind(State->GetInventory(), Visual, ControlledCharacter ? ControlledCharacter->GetMesh() : nullptr);
}

void AARPGPlayerController::ToggleInventory()
{
    StopMovementForUI();
    if (!InventoryWindow) InitializePlayerSystems();
    if (InventoryWindow) InventoryWindow->SetVisibility(InventoryWindow->GetVisibility() == ESlateVisibility::Collapsed ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}
void AARPGPlayerController::ShowInventoryMessage(const FText& Message)
{
    StopMovementForUI();
    if (!InventoryWindow) InitializePlayerSystems();
    if (InventoryWindow)
    {
        InventoryWindow->SetVisibility(ESlateVisibility::Visible);
        InventoryWindow->ShowMessage(Message);
    }
}
void AARPGPlayerController::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();
    InitializePlayerSystems();
}
void AARPGPlayerController::AcknowledgePossession(APawn* InPawn)
{
    Super::AcknowledgePossession(InPawn);
    InitializePlayerSystems();
}
void AARPGPlayerController::OnUnPossess()
{
    if (AARPGPlayerState* State = GetPlayerState<AARPGPlayerState>())
    {
        State->GetAbilitySystemComponent()->CancelAllAbilities();
        State->GetAbilitySystemComponent()->ClearActorInfo();
    }
    if (PlayerHUD) PlayerHUD->BindPlayerState(nullptr);
    if (InventoryWindow) InventoryWindow->BindInventory(nullptr);
    Super::OnUnPossess();
}
void AARPGPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (PlayerHUD) { PlayerHUD->RemoveFromParent(); PlayerHUD = nullptr; }
    if (InventoryWindow) { InventoryWindow->RemoveFromParent(); InventoryWindow = nullptr; }
    if (WorldDropTarget) { WorldDropTarget->RemoveFromParent(); WorldDropTarget = nullptr; }
    Super::EndPlay(EndPlayReason);
}
void AARPGPlayerController::ActivatePrimaryAbility()
{
    if (AARPGPlayerState* State = GetPlayerState<AARPGPlayerState>()) State->ActivatePrimaryAbility();
}
void AARPGPlayerController::DebugDamage()
{
    if (AARPGPlayerState* State = GetPlayerState<AARPGPlayerState>()) State->DebugDamage();
}
void AARPGPlayerController::DebugRestore()
{
    if (AARPGPlayerState* State = GetPlayerState<AARPGPlayerState>()) State->DebugRestore();
}
void AARPGPlayerController::AddInventoryTestItems()
{
#if !UE_BUILD_SHIPPING
    // TEMP: test supply shortcut. Remove when regular loot sources cover inventory testing.
    AARPGPlayerState* State = GetPlayerState<AARPGPlayerState>();
    if (!State || !HasAuthority()) return;
    int32 Added = 0;
    int32 Rejected = 0;
    for (const TSoftObjectPtr<UARPGItemDefinition>& Reference : InventoryTestDefinitions)
    {
        UARPGItemDefinition* Definition = Reference.LoadSynchronous();
        FGuid Id;
        FText Error;
        if (Definition && State->GetInventory()->TryAdd(Definition, Id, Error)) ++Added;
        else
        {
            ++Rejected;
            if (!Definition) UE_LOG(LogTemp, Error, TEXT("Missing inventory test definition: %s"), *Reference.ToString());
        }
    }
    ShowInventoryMessage(FText::Format(NSLOCTEXT("Inventory", "TestSupply", "Testföremål: {0} tillagda, {1} kunde inte läggas till. [K] ger en ny omgång."), FText::AsNumber(Added), FText::AsNumber(Rejected)));
#endif
}
void AARPGPlayerController::StopMovementForUI()
{
    bMoveHeld = false;
    PendingPickup.Reset();
    StopMovement();
}

void AARPGPlayerController::SetInventoryDragActive(bool bActive)
{
    StopMovementForUI();
    if (bActive && !WorldDropTarget && IsLocalController())
    {
        WorldDropTarget = CreateWidget<UARPGWorldDropTarget>(this);
        if (WorldDropTarget) WorldDropTarget->AddToPlayerScreen(5);
    }
    if (WorldDropTarget) WorldDropTarget->SetVisibility(bActive ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void AARPGPlayerController::DropInventoryItem(FGuid ItemId, UARPGInventoryComponent* Source)
{
    SetInventoryDragActive(false);
    AARPGPlayerState* State = GetPlayerState<AARPGPlayerState>();
    ACharacter* ControlledCharacter = Cast<ACharacter>(GetPawn());
    if (!HasAuthority() || !State || Source != State->GetInventory() || !ControlledCharacter) return;
    const FARPGInventoryItem* Entry = Source->FindItem(ItemId);
    if (!Entry) return;
    const FARPGInventoryItem Snapshot = *Entry;
    // TEMP: initial drop search dimensions; expose in authored loot settings when tuning placement.
    // Drop beside the player, not at an arbitrarily distant cursor location.
    const FVector Origin = ControlledCharacter->GetActorLocation();
    FCollisionQueryParams Query(SCENE_QUERY_STAT(InventoryDrop), false, ControlledCharacter);
    bool bFoundGround = false;
    FVector Ground = FVector::ZeroVector;
    for (int32 Index = 0; Index < 8; ++Index)
    {
        const FVector Direction = ControlledCharacter->GetActorRightVector().RotateAngleAxis(Index * 45.f, FVector::UpVector);
        const FVector Candidate = Origin + Direction * 150.f;
        FHitResult SideHit, FloorHit;
        if (GetWorld()->LineTraceSingleByChannel(SideHit, Origin, Candidate, ECC_Visibility, Query)) continue;
        if (!GetWorld()->LineTraceSingleByChannel(FloorHit, Candidate + FVector(0.f, 0.f, 100.f), Candidate - FVector(0.f, 0.f, 250.f), ECC_Visibility, Query)) continue;
        if (!ControlledCharacter->GetCharacterMovement()->IsWalkable(FloorHit)) continue;
        Ground = FloorHit.ImpactPoint;
        bFoundGround = true;
        break;
    }
    if (!bFoundGround)
    {
        ShowInventoryMessage(NSLOCTEXT("Inventory", "NoDropGround", "Ingen säker markyta bredvid spelaren. Föremålet ligger kvar."));
        return;
    }
    AARPGDroppedItem* Dropped = GetWorld()->SpawnActor<AARPGDroppedItem>(Ground, FRotator::ZeroRotator);
    if (!Dropped) { ShowInventoryMessage(NSLOCTEXT("Inventory", "SpawnFailed", "Kunde inte skapa markdrop. Föremålet ligger kvar.")); return; }
    Dropped->SetActorHiddenInGame(true);
    UChestLootWidget* Label = Dropped->InitializeItem(Snapshot, this);
    FText Error;
    if (!Label || !Source->TryRemove(ItemId, Error))
    {
        Dropped->Destroy();
        ShowInventoryMessage(Error.IsEmpty() ? NSLOCTEXT("Inventory", "LabelFailed", "Kunde inte skapa lootskylt. Föremålet ligger kvar.") : Error);
        return;
    }
    Dropped->SetActorHiddenInGame(false);
    LootLabels.RemoveAll([](const TWeakObjectPtr<UChestLootWidget>& LabelRef) { return !LabelRef.IsValid(); });
    LootLabels.Add(Label);
    ShowInventoryMessage(NSLOCTEXT("Inventory", "Dropped", "Föremålet ligger på marken bredvid dig."));
}

void AARPGPlayerController::BeginMove()
{
    PendingPickup.Reset();
    bMoveHeld = true;
    TimeSinceDestinationUpdate = 0.f;
    MoveToCursor();
}
void AARPGPlayerController::EndMove() { bMoveHeld = false; }

void AARPGPlayerController::PlayerTick(float DeltaSeconds)
{
    Super::PlayerTick(DeltaSeconds);
    if (UChestLootWidget* Loot = PendingPickup.Get())
    {
        PickupElapsed += DeltaSeconds;
        APawn* ControlledPawn = GetPawn();
        if (!ControlledPawn || !Loot->IsAvailable() || PickupElapsed > 30.f)
        {
            PendingPickup.Reset();
            StopMovement();
        }
        else if (ControlledPawn->GetDistanceTo(Loot->Item.Get()) <= PickupDistance)
        {
            StopMovement();
            Loot->TryPickup(ControlledPawn);
            PendingPickup.Reset();
        }
    }
    // A release outside the viewport must not leave continuous movement enabled.
    if (bMoveHeld && !IsInputKeyDown(EKeys::LeftMouseButton)) bMoveHeld = false;
    if (bMoveHeld)
    {
        TimeSinceDestinationUpdate += DeltaSeconds;
        if (TimeSinceDestinationUpdate >= DestinationUpdateInterval)
        {
            TimeSinceDestinationUpdate = 0.f;
            MoveToCursor();
        }
    }
}

void AARPGPlayerController::MoveToCursor()
{
    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn) return;
    FHitResult Hit;
    if (!GetHitResultUnderCursor(ECC_Visibility, false, Hit) || !Hit.bBlockingHit) return;
    UNavigationSystemV1* Nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    FNavLocation Destination;
    if (Nav && Nav->ProjectPointToNavigation(Hit.ImpactPoint, Destination, FVector(100.f, 100.f, 150.f), &ControlledPawn->GetNavAgentPropertiesRef()))
    {
        UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, Destination.Location);
    }
}

void AARPGPlayerController::PickupNearbyItem()
{
    StopMovementForUI();
    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn) return;
    UChestLootWidget* Nearest = nullptr;
    float BestDistance = PickupDistance;
    for (const TWeakObjectPtr<UChestLootWidget>& Label : LootLabels)
        if (Label.IsValid() && Label->IsAvailable())
        {
            const float Distance = ControlledPawn->GetDistanceTo(Label->Item.Get());
            if (Distance <= BestDistance) { BestDistance = Distance; Nearest = Label.Get(); }
        }
    if (Nearest) Nearest->TryPickup(ControlledPawn);
    else ShowInventoryMessage(NSLOCTEXT("Inventory", "UseInventory", "Ingen loot inom räckhåll. Utrusta och ta av föremål här i inventoryt."));
}

void AARPGPlayerController::InitializeLootLabels()
{
    TArray<AActor*> Items, Plates, Stems;
    UGameplayStatics::GetAllActorsWithTag(this, TEXT("ChestArmor"), Items);
    UGameplayStatics::GetAllActorsWithTag(this, TEXT("ChestDropPlate"), Plates);
    UGameplayStatics::GetAllActorsWithTag(this, TEXT("ChestDropStem"), Stems);
    // TEMP: map adapter for one existing chest. Replace tags with authored pickup actors
    // holding explicit item definitions before adding multiple world drops.
    if (Items.Num() != 1 || Plates.Num() != 1 || Stems.Num() > 1)
    {
        UE_LOG(LogTemp, Warning, TEXT("Loot click setup requires one unambiguous chest/plate pair."));
        return;
    }
    UWidgetComponent* Component = Plates[0]->FindComponentByClass<UWidgetComponent>();
    if (!Component) return;
    Component->InitWidget();
    UUserWidget* ExistingLabel = Component->GetUserWidgetObject();
    if (!ExistingLabel || Cast<UChestLootWidget>(ExistingLabel)) return;
    UARPGItemDefinition* Definition = ChestItemDefinition.LoadSynchronous();
    if (!Definition)
    {
        UE_LOG(LogTemp, Error, TEXT("Chest Data Asset missing: %s. Check ARPGPlayerController settings in DefaultGame.ini."), *ChestItemDefinition.ToString());
        return;
    }
    UChestLootWidget* Clickable = CreateWidget<UChestLootWidget>(this);
    Clickable->Definition = Definition;
    Clickable->Item = Items[0];
    Clickable->Plate = Plates[0];
    if (Stems.Num() == 1) Clickable->Stem = Stems[0];
    Clickable->SetLabel(ExistingLabel);
    Component->SetWidget(Clickable);
    LootLabels.Add(Clickable);
    UE_LOG(LogTemp, Display, TEXT("Loot click label bound to %s"), *Items[0]->GetName());
}

void AARPGPlayerController::RequestPickup(UChestLootWidget* Loot)
{
    bMoveHeld = false;
    PendingPickup.Reset();
    StopMovement();
    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn || !Loot || !Loot->IsAvailable()) return;
    if (ControlledPawn->GetDistanceTo(Loot->Item.Get()) <= PickupDistance)
    {
        Loot->TryPickup(ControlledPawn);
        return;
    }
    UNavigationSystemV1* Nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (!Nav) return;
    const FVector ItemLocation = Loot->Item->GetActorLocation();
    const FVector Start = ControlledPawn->GetNavAgentLocation();
    const FVector CenterOffset = ControlledPawn->GetActorLocation() - Start;
    float BestLength = TNumericLimits<float>::Max();
    FVector BestDestination = FVector::ZeroVector;
    // Test reachable points inside the interaction radius, not the item's collider.
    for (int32 Index = 0; Index < 17; ++Index)
    {
        const float Angle = Index * 2.f * PI / 16.f;
        const FVector Offset = Index == 16 ? FVector::ZeroVector : FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.f) * PickupDistance * 0.65f;
        FNavLocation Projected;
        if (!Nav->ProjectPointToNavigation(ItemLocation + Offset, Projected, FVector(80.f, 80.f, 300.f), &ControlledPawn->GetNavAgentPropertiesRef())) continue;
        if (FVector::Dist(Projected.Location + CenterOffset, ItemLocation) > PickupDistance - 40.f) continue;
        UNavigationPath* Path = Nav->FindPathToLocationSynchronously(this, Start, Projected.Location, ControlledPawn);
        if (!Path || !Path->IsValid() || Path->IsPartial() || Path->GetPathLength() >= BestLength) continue;
        BestLength = Path->GetPathLength();
        BestDestination = Projected.Location;
    }
    if (BestLength == TNumericLimits<float>::Max())
    {
        UE_LOG(LogTemp, Display, TEXT("No reachable pickup position for %s"), *Loot->Item->GetName());
        return;
    }
    PendingPickup = Loot;
    PickupElapsed = 0.f;
    UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, BestDestination);
}
