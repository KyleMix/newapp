#include "HW_ModerationServiceSubsystem.h"

#include "Engine/Engine.h"

bool UHW_ModerationServiceSubsystem::IsPlayerMutedLocal(const FString& PlayerId) const
{
    return bMuteAllInRoom || MutedPlayerIds.Contains(PlayerId);
}

bool UHW_ModerationServiceSubsystem::IsPlayerBlockedLocal(const FString& PlayerId) const
{
    return BlockedPlayerIds.Contains(PlayerId);
}

void UHW_ModerationServiceSubsystem::SetPlayerMutedLocal(const FString& PlayerId, bool bMuted)
{
    if (bMuted)
    {
        MutedPlayerIds.Add(PlayerId);
    }
    else
    {
        MutedPlayerIds.Remove(PlayerId);
    }
}

void UHW_ModerationServiceSubsystem::SetPlayerBlockedLocal(const FString& PlayerId, bool bBlocked)
{
    if (bBlocked)
    {
        BlockedPlayerIds.Add(PlayerId);
    }
    else
    {
        BlockedPlayerIds.Remove(PlayerId);
    }
}

void UHW_ModerationServiceSubsystem::ReportPlayer(const FString& PlayerId, const FString& Reason)
{
    UE_LOG(LogTemp, Warning, TEXT("[ReportPlaceholder] PlayerId=%s Reason=%s"), *PlayerId, *Reason);
}

void UHW_ModerationServiceSubsystem::SetMuteAllInRoom(bool bMuteAll)
{
    bMuteAllInRoom = bMuteAll;
}
