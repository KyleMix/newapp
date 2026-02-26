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
