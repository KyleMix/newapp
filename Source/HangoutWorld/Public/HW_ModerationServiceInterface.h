#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HW_ModerationServiceInterface.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UHW_ModerationServiceInterface : public UInterface
{
    GENERATED_BODY()
};

class HANGOUTWORLD_API IHW_ModerationServiceInterface
{
    GENERATED_BODY()

public:
    virtual bool IsPlayerMutedLocal(const FString& PlayerId) const = 0;
    virtual bool IsPlayerBlockedLocal(const FString& PlayerId) const = 0;
    virtual void SetPlayerMutedLocal(const FString& PlayerId, bool bMuted) = 0;
    virtual void SetPlayerBlockedLocal(const FString& PlayerId, bool bBlocked) = 0;
    virtual void ReportPlayer(const FString& PlayerId, const FString& Reason) = 0;
    virtual void SetMuteAllInRoom(bool bMuteAll) = 0;
};
