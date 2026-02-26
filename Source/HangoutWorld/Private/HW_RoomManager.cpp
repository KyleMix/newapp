#include "HW_RoomManager.h"

#include "Engine/World.h"
#include "GameFramework/PlayerState.h"
#include "HW_FurnitureCatalogDataAsset.h"
#include "HW_PlaceableFurnitureActor.h"
#include "HW_PlayerController.h"
#include "HW_RoomJsonUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogHWRoom, Log, All);

AHW_RoomManager::AHW_RoomManager()
{
    bReplicates = true;
    SetReplicateMovement(false);
}

void AHW_RoomManager::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        if (RoomOwnerUniqueId.IsEmpty())
        {
            RoomOwnerUniqueId = GetWorld() ? GetWorld()->URL.GetOption(TEXT("OwnerId="), TEXT("")) : TEXT("");
            if (RoomOwnerUniqueId.IsEmpty())
            {
                RoomOwnerUniqueId = TEXT("OfflineOwner");
            }
        }

        LoadLayoutFromDisk();
    }
}

void AHW_RoomManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AHW_RoomManager, RoomLayout);
    DOREPLIFETIME(AHW_RoomManager, RoomOwnerUniqueId);
}

bool AHW_RoomManager::CanControllerEdit(const AHW_PlayerController* Controller) const
{
    if (!Controller)
    {
        return false;
    }

    const APlayerState* PlayerState = Controller->PlayerState;
    if (!PlayerState)
    {
        return false;
    }

    const FString NetId = PlayerState->GetUniqueId().ToString();
    if (!NetId.IsEmpty() && NetId != TEXT("INVALID"))
    {
        return NetId == RoomOwnerUniqueId;
    }

    return PlayerState->GetPlayerName() == RoomOwnerUniqueId;
}

bool AHW_RoomManager::AddFurnitureFromRequest(AHW_PlayerController* RequestingController, FName CatalogId, const FTransform& Transform)
{
    if (!HasAuthority() || !CanControllerEdit(RequestingController) || !FurnitureCatalog)
    {
        UE_LOG(LogHWRoom, Warning, TEXT("AddFurniture denied: authority=%d canEdit=%d catalogValid=%d"), HasAuthority() ? 1 : 0, CanControllerEdit(RequestingController) ? 1 : 0, FurnitureCatalog ? 1 : 0);
        return false;
    }

    FHWCatalogItemDefinition ItemDefinition;
    if (!FurnitureCatalog->FindItemById(CatalogId, ItemDefinition))
    {
        UE_LOG(LogTemp, Warning, TEXT("Skipped unknown catalog id '%s'"), *CatalogId.ToString());
        return false;
    }

    FHWPlacedFurnitureRecord& NewRecord = RoomLayout.AddDefaulted_GetRef();
    NewRecord.ItemId = FGuid::NewGuid();
    NewRecord.CatalogId = CatalogId;
    NewRecord.Transform = Transform;

    SpawnFurnitureActorFromRecord(NewRecord);
    ForceNetUpdate();
    SaveLayoutToDisk();
    return true;
}

bool AHW_RoomManager::RemoveFurnitureFromRequest(AHW_PlayerController* RequestingController, const FGuid& ItemId)
{
    if (!HasAuthority() || !CanControllerEdit(RequestingController))
    {
        UE_LOG(LogHWRoom, Warning, TEXT("RemoveFurniture denied: authority=%d canEdit=%d"), HasAuthority() ? 1 : 0, CanControllerEdit(RequestingController) ? 1 : 0);
        return false;
    }

    const int32 Removed = RoomLayout.RemoveAll([ItemId](const FHWPlacedFurnitureRecord& Record)
    {
        return Record.ItemId == ItemId;
    });

    if (Removed <= 0)
    {
        return false;
    }

    if (TObjectPtr<AHW_PlaceableFurnitureActor>* Existing = SpawnedActors.Find(ItemId))
    {
        if (*Existing)
        {
            (*Existing)->Destroy();
        }
        SpawnedActors.Remove(ItemId);
    }

    ForceNetUpdate();
    SaveLayoutToDisk();
    return true;
}

bool AHW_RoomManager::SaveLayoutToDisk()
{
    if (RoomOwnerUniqueId.IsEmpty())
    {
        UE_LOG(LogHWRoom, Warning, TEXT("SaveLayoutToDisk skipped: empty RoomOwnerUniqueId."));
        return false;
    }

    const bool bSaved = FHW_RoomJsonUtils::SaveLayout(RoomOwnerUniqueId, RoomLayout);
    if (!bSaved)
    {
        UE_LOG(LogHWRoom, Warning, TEXT("SaveLayoutToDisk failed for owner '%s'"), *RoomOwnerUniqueId);
    }
    return bSaved;
}

void AHW_RoomManager::LoadLayoutFromDisk()
{
    if (!HasAuthority())
    {
        return;
    }

    TArray<FHWPlacedFurnitureRecord> LoadedLayout;
    const bool bLoaded = FHW_RoomJsonUtils::LoadLayout(RoomOwnerUniqueId, LoadedLayout);
    if (!bLoaded)
    {
        UE_LOG(LogHWRoom, Warning, TEXT("LoadLayoutFromDisk failed for owner '%s'. Starting empty layout."), *RoomOwnerUniqueId);
        RoomLayout.Reset();
    }
    else
    {
        RoomLayout = LoadedLayout;
        UE_LOG(LogHWRoom, Log, TEXT("Loaded %d furniture records for owner '%s'"), RoomLayout.Num(), *RoomOwnerUniqueId);
    }

    ReconcileSpawnedActorsWithLayout();
    ForceNetUpdate();
}

void AHW_RoomManager::OnRep_Layout()
{
    ReconcileSpawnedActorsWithLayout();
}

void AHW_RoomManager::ReconcileSpawnedActorsWithLayout()
{
    TSet<FGuid> DesiredIds;
    for (const FHWPlacedFurnitureRecord& Record : RoomLayout)
    {
        DesiredIds.Add(Record.ItemId);
        if (!SpawnedActors.Contains(Record.ItemId))
        {
            SpawnFurnitureActorFromRecord(Record);
        }
    }

    TArray<FGuid> ExistingIds;
    SpawnedActors.GenerateKeyArray(ExistingIds);
    for (const FGuid& ExistingId : ExistingIds)
    {
        if (!DesiredIds.Contains(ExistingId))
        {
            if (TObjectPtr<AHW_PlaceableFurnitureActor>* ExistingActor = SpawnedActors.Find(ExistingId))
            {
                if (*ExistingActor)
                {
                    (*ExistingActor)->Destroy();
                }
            }
            SpawnedActors.Remove(ExistingId);
        }
    }
}

AHW_PlaceableFurnitureActor* AHW_RoomManager::SpawnFurnitureActorFromRecord(const FHWPlacedFurnitureRecord& Record)
{
    if (!GetWorld() || !FurnitureCatalog)
    {
        return nullptr;
    }

    FHWCatalogItemDefinition CatalogItem;
    if (!FurnitureCatalog->FindItemById(Record.CatalogId, CatalogItem))
    {
        UE_LOG(LogTemp, Warning, TEXT("Skipped spawn for unknown catalog id '%s'"), *Record.CatalogId.ToString());
        return nullptr;
    }

    UClass* FurnitureClass = CatalogItem.PlaceableClass.LoadSynchronous();
    if (!FurnitureClass)
    {
        return nullptr;
    }

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    AHW_PlaceableFurnitureActor* Spawned = GetWorld()->SpawnActor<AHW_PlaceableFurnitureActor>(FurnitureClass, Record.Transform, Params);
    if (!Spawned)
    {
        return nullptr;
    }

    Spawned->PlacedItemId = Record.ItemId;
    Spawned->CatalogItemId = Record.CatalogId;
    Spawned->RoomOwnerUniqueId = RoomOwnerUniqueId;
    SpawnedActors.Add(Record.ItemId, Spawned);
    return Spawned;
}
