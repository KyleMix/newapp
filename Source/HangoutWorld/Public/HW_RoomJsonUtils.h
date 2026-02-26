#pragma once

#include "CoreMinimal.h"
#include "HW_Types.h"

class HANGOUTWORLD_API FHW_RoomJsonUtils
{
public:
    static FString GetLayoutFilePath(const FString& OwnerUniqueId);
    static bool SaveLayout(const FString& OwnerUniqueId, const TArray<FHWPlacedFurnitureRecord>& Records);
    static bool LoadLayout(const FString& OwnerUniqueId, TArray<FHWPlacedFurnitureRecord>& OutRecords);
};
