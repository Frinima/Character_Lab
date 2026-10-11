#include "PrepareARPGCommandlet.h"
#include "ARPGGameMode.h"
#include "Builders/CubeBuilder.h"
#include "ActorFactories/ActorFactory.h"
#include "Components/BrushComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "FileHelpers.h"
#include "GameFramework/WorldSettings.h"
#include "HAL/FileManager.h"
#include "Misc/PackageName.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "NavigationSystem.h"
#include "NavigationData.h"
#include "NavigationPath.h"
#include "GameFramework/PlayerStart.h"
#include "Misc/Parse.h"
#include "WorldPartition/LoaderAdapter/LoaderAdapterShape.h"

UPrepareARPGCommandlet::UPrepareARPGCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}

int32 UPrepareARPGCommandlet::Main(const FString& Params)
{
    const FString Destination(TEXT("/Game/ARPG/Lvl_ARPG"));
    const bool bRebuildNavigation = FParse::Param(*Params, TEXT("RebuildNavigation"));
    if (FPackageName::DoesPackageExist(Destination) && !bRebuildNavigation)
    {
        UE_LOG(LogTemp, Error, TEXT("ARPG map already exists; refusing to overwrite it."));
        return 1;
    }
    UWorld* World = UEditorLoadingAndSavingUtils::LoadMap(bRebuildNavigation ? Destination : TEXT("/Game/ThirdPerson/Lvl_ThirdPerson"));
    if (!World) return 2;
    FLoaderAdapterShape LoadedRegion(World, FBox(FVector(-100000.f), FVector(100000.f)), TEXT("ARPG preparation"));
    LoadedRegion.Load();
    FlushAsyncLoading();
    World->GetWorldSettings()->DefaultGameMode = AARPGGameMode::StaticClass();

    FBox Bounds(ForceInit);
    for (TActorIterator<AActor> It(World); It; ++It)
    {
        const FBox ActorBounds = It->GetComponentsBoundingBox(true);
        // Exclude distant sky geometry from the navigation build volume.
        if (ActorBounds.IsValid && ActorBounds.GetExtent().GetMax() < 20000.f)
            Bounds += ActorBounds;
    }
    if (!Bounds.IsValid) return 3;
    Bounds = Bounds.ExpandBy(FVector(200.f, 200.f, 300.f));
    ANavMeshBoundsVolume* Volume = nullptr;
    for (TActorIterator<ANavMeshBoundsVolume> It(World); It; ++It) { Volume = *It; break; }
    if (!Volume) Volume = World->SpawnActor<ANavMeshBoundsVolume>();
    Volume->SetActorLabel(TEXT("ARPG_NavigationBounds"));
    UCubeBuilder* Builder = NewObject<UCubeBuilder>(Volume);
    const FVector Size = Bounds.GetSize();
    Builder->X = Size.X;
    Builder->Y = Size.Y;
    Builder->Z = Size.Z;
    UActorFactory::CreateBrushForVolumeActor(Volume, Builder);
    Volume->SetActorLocation(Bounds.GetCenter());
    Volume->GetBrushComponent()->SetCanEverAffectNavigation(false);
    Volume->PostEditChange();
    if (UNavigationSystemV1* Nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World))
    {
        Nav->OnNavigationBoundsUpdated(Volume);
        FlushAsyncLoading();
        Nav->RemoveNavigationBuildLock(ENavigationBuildLock::AsyncLoadLock, UNavigationSystemV1::ELockRemovalRebuildAction::NoRebuild);
        Nav->Build();
        for (TActorIterator<ANavigationData> It(World); It; ++It)
            It->EnsureBuildCompletion();
        bool bValidPath = false;
        for (TActorIterator<APlayerStart> It(World); It; ++It)
        {
            UE_LOG(LogTemp, Display, TEXT("Checking PlayerStart at %s"), *It->GetActorLocation().ToString());
            FNavLocation Start, End;
            if (Nav->ProjectPointToNavigation(It->GetActorLocation(), Start, FVector(100.f, 100.f, 500.f)))
            {
                for (int32 Attempt = 0; Attempt < 20 && !bValidPath; ++Attempt)
                {
                    if (!Nav->GetRandomReachablePointInRadius(Start.Location, 800.f, End)) continue;
                    if (FVector::Dist2D(Start.Location, End.Location) < 150.f) continue;
                    UNavigationPath* Path = UNavigationSystemV1::FindPathToLocationSynchronously(World, Start.Location, End.Location);
                    bValidPath = Path && Path->IsValid() && !Path->IsPartial() && Path->PathPoints.Num() > 1;
                    if (bValidPath) UE_LOG(LogTemp, Display, TEXT("ARPG navigation verified: %s -> %s (%d path points)"), *Start.Location.ToString(), *End.Location.ToString(), Path->PathPoints.Num());
                }
            }
        }
        if (!bValidPath)
        {
            UE_LOG(LogTemp, Error, TEXT("No usable navigation path from PlayerStart."));
            return 7;
        }
    }
    else return 6;
    if (!UEditorLoadingAndSavingUtils::SaveMap(World, Destination)) return 5;
    UE_LOG(LogTemp, Display, TEXT("ARPG map saved: %s; navigation bounds %s"), *Destination, *Bounds.ToString());
    return 0;
}
