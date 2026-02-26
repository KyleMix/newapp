#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HW_Types.h"
#include "HW_FurnitureCatalogDataAsset.generated.h"

UCLASS(BlueprintType)
class HANGOUTWORLD_API UHW_FurnitureCatalogDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hangout|Rooms")
    TArray<FHWCatalogItemDefinition> Items;

    UFUNCTION(BlueprintPure, Category = "Hangout|Rooms")
    bool FindItemById(FName CatalogId, FHWCatalogItemDefinition& OutDefinition) const;
};
