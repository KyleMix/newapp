#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "HW_ModerationServiceInterface.h"
#include "HW_ModerationServiceSubsystem.generated.h"

UCLASS()
class HANGOUTWORLD_API UHW_ModerationServiceSubsystem : public UGameInstanceSubsystem, public IHW_ModerationServiceInterface
{
    GENERATED_BODY()

public:
    virtual bool IsPlayerMutedLocal(const FString& PlayerId) const override;
    virtual bool IsPlayerBlockedLocal(const FString& PlayerId) const override;
    virtual void SetPlayerMutedLocal(const FString& PlayerId, bool bMuted) override;
    virtual void SetPlayerBlockedLocal(const FString& PlayerId, bool bBlocked) override;
    virtual void ReportPlayer(const FString& PlayerId, const FString& Reason) override;
    virtual void SetMuteAllInRoom(bool bMuteAll) override;

    UFUNCTION(BlueprintPure, Category = "Hangout|Moderation")
    bool IsMuteAllInRoomEnabled() const { return bMuteAllInRoom; }

private:
    UPROPERTY()
    TSet<FString> MutedPlayerIds;

    UPROPERTY()
    TSet<FString> BlockedPlayerIds;

    UPROPERTY()
    bool bMuteAllInRoom = false;
};
