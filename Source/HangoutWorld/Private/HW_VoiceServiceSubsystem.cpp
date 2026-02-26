#include "HW_VoiceServiceSubsystem.h"

#include "HW_GameInstance.h"

void UHW_VoiceServiceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    if (UHW_GameInstance* GI = GetGameInstance<UHW_GameInstance>())
    {
        GI->OnVoiceChatEnabledChanged.AddDynamic(this, &UHW_VoiceServiceSubsystem::HandleVoiceToggleChanged);

        if (GI->IsVoiceChatEnabled())
        {
            InitializeVoice();
        }
    }
}

void UHW_VoiceServiceSubsystem::Deinitialize()
{
    ShutdownVoice();

    if (UHW_GameInstance* GI = GetGameInstance<UHW_GameInstance>())
    {
        GI->OnVoiceChatEnabledChanged.RemoveDynamic(this, &UHW_VoiceServiceSubsystem::HandleVoiceToggleChanged);
    }

    Super::Deinitialize();
}

void UHW_VoiceServiceSubsystem::InitializeVoice()
{
    if (bVoiceInitialized)
    {
        return;
    }

    // TODO: Initialize OnlineSubsystem voice or third-party backend adapter.
    bVoiceInitialized = true;
}

void UHW_VoiceServiceSubsystem::ShutdownVoice()
{
    if (!bVoiceInitialized)
    {
        return;
    }

    // TODO: Shutdown selected voice backend and release microphone capture resources.
    bVoiceInitialized = false;
    bPushToTalkActive = false;
}

bool UHW_VoiceServiceSubsystem::IsVoiceAvailable() const
{
    return bVoiceInitialized;
}

void UHW_VoiceServiceSubsystem::SetPushToTalkActive(bool bActive)
{
    if (!bVoiceInitialized)
    {
        bPushToTalkActive = false;
        return;
    }

    if (bPushToTalkActive == bActive)
    {
        return;
    }

    // TODO: Call into backend to start/stop local voice transmission for push-to-talk.
    bPushToTalkActive = bActive;
}

void UHW_VoiceServiceSubsystem::HandleVoiceToggleChanged(bool bEnabled)
{
    if (bEnabled)
    {
        InitializeVoice();
    }
    else
    {
        ShutdownVoice();
    }
}
