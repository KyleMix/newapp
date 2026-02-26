#include "HW_PlayerState.h"

#include "Net/UnrealNetwork.h"

AHW_PlayerState::AHW_PlayerState()
{
    DisplayName = TEXT("Player");
}

void AHW_PlayerState::ServerSetDisplayName_Implementation(const FString& NewName)
{
    FString Sanitized = NewName.Left(24).TrimStartAndEnd();
    if (Sanitized.IsEmpty())
    {
        Sanitized = TEXT("Player");
    }

    DisplayName = Sanitized;
    OnRep_DisplayName();
}

bool AHW_PlayerState::CanSendChatMessage(float WindowSeconds, int32 MaxMessagesInWindow)
{
    const UWorld* World = GetWorld();
    if (!World)
    {
        return false;
    }

    const float Now = World->GetTimeSeconds();

    RecentChatServerTimes.RemoveAll([Now, WindowSeconds](const float Timestamp)
    {
        return (Now - Timestamp) > WindowSeconds;
    });

    if (RecentChatServerTimes.Num() >= MaxMessagesInWindow)
    {
        return false;
    }

    RecentChatServerTimes.Add(Now);
    return true;
}

void AHW_PlayerState::OnRep_DisplayName()
{
    OnDisplayNameChanged.Broadcast();
}

void AHW_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AHW_PlayerState, DisplayName);
}
