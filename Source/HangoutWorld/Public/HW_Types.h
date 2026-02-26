#pragma once

#include "CoreMinimal.h"
#include "HW_Types.generated.h"

UENUM(BlueprintType)
enum class EHWEmoteType : uint8
{
    Wave,
    Point
};

USTRUCT(BlueprintType)
struct FHWChatMessage
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString SenderName;

    UPROPERTY(BlueprintReadOnly)
    FString Message;

    UPROPERTY(BlueprintReadOnly)
    FString Timestamp;
};

USTRUCT(BlueprintType)
struct FHWCatalogItemDefinition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hangout|Rooms")
    FName CatalogId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hangout|Rooms")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hangout|Rooms")
    TSoftClassPtr<AActor> PlaceableClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hangout|Rooms")
    FTransform DefaultTransform = FTransform::Identity;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hangout|Rooms")
    float GridSnapSize = 25.f;
};

USTRUCT(BlueprintType)
struct FHWPlacedFurnitureRecord
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hangout|Rooms")
    FGuid ItemId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hangout|Rooms")
    FName CatalogId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hangout|Rooms")
    FTransform Transform = FTransform::Identity;
};
