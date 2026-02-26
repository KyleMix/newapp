#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HW_Types.h"
#include "HW_RoomManager.generated.h"

class AHW_PlaceableFurnitureActor;
class AHW_PlayerController;
class UHW_FurnitureCatalogDataAsset;

UCLASS()
class HANGOUTWORLD_API AHW_RoomManager : public AActor
{
    GENERATED_BODY()

public:
    AHW_RoomManager();

    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(BlueprintCallable, Category = "Hangout|Rooms")
    bool CanControllerEdit(const AHW_PlayerController* Controller) const;

    UFUNCTION(BlueprintCallable, Category = "Hangout|Rooms")
    bool AddFurnitureFromRequest(AHW_PlayerController* RequestingController, FName CatalogId, const FTransform& Transform);

    UFUNCTION(BlueprintCallable, Category = "Hangout|Rooms")
    bool RemoveFurnitureFromRequest(AHW_PlayerController* RequestingController, const FGuid& ItemId);

    UFUNCTION(BlueprintCallable, Category = "Hangout|Rooms")
    bool SaveLayoutToDisk();

    UFUNCTION(BlueprintCallable, Category = "Hangout|Rooms")
    void LoadLayoutFromDisk();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hangout|Rooms")
    TObjectPtr<UHW_FurnitureCatalogDataAsset> FurnitureCatalog;

    UPROPERTY(ReplicatedUsing = OnRep_Layout, BlueprintReadOnly, Category = "Hangout|Rooms")
    TArray<FHWPlacedFurnitureRecord> RoomLayout;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hangout|Rooms")
    FString RoomOwnerUniqueId;

protected:
    UFUNCTION()
    void OnRep_Layout();

    void ReconcileSpawnedActorsWithLayout();
    AHW_PlaceableFurnitureActor* SpawnFurnitureActorFromRecord(const FHWPlacedFurnitureRecord& Record);

    TMap<FGuid, TObjectPtr<AHW_PlaceableFurnitureActor>> SpawnedActors;
};
