#include "HW_GameState.h"
#include "Net/UnrealNetwork.h"

void AHW_GameState::AddChatMessage(const FString& SenderName, const FString& Message)
{
    if (!HasAuthority())
    {
        return;
    }

    FHWChatMessage& Entry = ChatMessages.AddDefaulted_GetRef();
    Entry.SenderName = SenderName;
    Entry.Message = Message.Left(256);
    Entry.Timestamp = FDateTime::UtcNow().ToString(TEXT("%H:%M:%S"));

    if (ChatMessages.Num() > 100)
    {
        ChatMessages.RemoveAt(0);
    }

    OnRep_ChatMessages();
}

void AHW_GameState::OnRep_ChatMessages()
{
    OnChatMessagesUpdated.Broadcast(ChatMessages);
}

void AHW_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AHW_GameState, ChatMessages);
}
