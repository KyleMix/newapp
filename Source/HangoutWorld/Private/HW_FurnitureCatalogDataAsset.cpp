#include "HW_FurnitureCatalogDataAsset.h"

bool UHW_FurnitureCatalogDataAsset::FindItemById(FName CatalogId, FHWCatalogItemDefinition& OutDefinition) const
{
    if (const FHWCatalogItemDefinition* Found = Items.FindByPredicate([CatalogId](const FHWCatalogItemDefinition& Item)
    {
        return Item.CatalogId == CatalogId;
    }))
    {
        OutDefinition = *Found;
        return true;
    }

    return false;
}
