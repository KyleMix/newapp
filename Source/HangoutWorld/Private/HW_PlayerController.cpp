#include "HW_PlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "Components/InputComponent.h"
#include "HW_Character.h"
#include "HW_GameInstance.h"
#include "HW_GameState.h"
#include "HW_ModerationServiceSubsystem.h"
#include "HW_PlayerState.h"
#include "HW_VoiceServiceSubsystem.h"

void AHW_PlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (IsLocalController() && !IsRunningDedicatedServer() && LobbyWidgetClass)
    {
        LobbyWidget = CreateWidget(this, LobbyWidgetClass);
        if (LobbyWidget)
        {
            LobbyWidget->AddToViewport();
        }
    }

    if (IsLocalController())
    {
        if (UHW_GameInstance* GI = GetGameInstance<UHW_GameInstance>())
        {
            if (AHW_PlayerState* HWPS = GetPlayerState<AHW_PlayerState>())
            {
                HWPS->ServerSetDisplayName(GI->LocalPlayerName);
            }
        }
    }
}

void AHW_PlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (InputComponent)
    {
        InputComponent->BindAction(TEXT("PushToTalk"), IE_Pressed, this, &AHW_PlayerController::HandlePushToTalkPressed);
        InputComponent->BindAction(TEXT("PushToTalk"), IE_Released, this, &AHW_PlayerController::HandlePushToTalkReleased);
    }
}

void AHW_PlayerController::SendChatMessage(const FString& Message)
{
    const FString Sanitized = SanitizeChatMessage(Message);
    if (!Sanitized.IsEmpty())
    {
        ServerSendChatMessage(Sanitized);
    }
}

void AHW_PlayerController::ServerSendChatMessage_Implementation(const FString& Message)
{
    AHW_PlayerState* HWPlayerState = GetPlayerState<AHW_PlayerState>();
    AHW_GameState* HWGameState = GetWorld() ? GetWorld()->GetGameState<AHW_GameState>() : nullptr;
    if (!HWPlayerState || !HWGameState)
    {
        return;
    }

    if (!HWPlayerState->CanSendChatMessage(5.f, 5))
    {
        return;
    }

    const FString Sanitized = SanitizeChatMessage(Message);
    if (Sanitized.IsEmpty())
    {
        return;
    }

    HWGameState->AddChatMessage(HWPlayerState->GetDisplayName(), Sanitized);
}

void AHW_PlayerController::TriggerEmote(EHWEmoteType Emote)
{
    if (AHW_Character* HWCharacter = GetPawn<AHW_Character>())
    {
        HWCharacter->ServerPlayEmote(Emote);
    }
}

void AHW_PlayerController::SetChatFocus(bool bFocused)
{
    if (bFocused)
    {
        FInputModeGameAndUI Mode;
        Mode.SetHideCursorDuringCapture(false);
        SetInputMode(Mode);
        bShowMouseCursor = true;
    }
    else
    {
        SetInputMode(FInputModeGameOnly());
        bShowMouseCursor = false;
    }
}

void AHW_PlayerController::SetVoiceEnabledInSettings(bool bEnabled)
{
    if (UHW_GameInstance* GI = GetGameInstance<UHW_GameInstance>())
    {
        GI->SetVoiceChatEnabled(bEnabled);
    }
}

bool AHW_PlayerController::IsVoiceEnabledInSettings() const
{
    if (const UHW_GameInstance* GI = GetGameInstance<UHW_GameInstance>())
    {
        return GI->IsVoiceChatEnabled();
    }

    return false;
}

void AHW_PlayerController::MutePlayerLocal(AHW_PlayerState* TargetPlayer, bool bMuted)
{
    if (GetGameInstance() && (UHW_ModerationServiceSubsystem* Moderation = GetGameInstance()->GetSubsystem<UHW_ModerationServiceSubsystem>()))
    {
        Moderation->SetPlayerMutedLocal(ResolvePlayerId(TargetPlayer), bMuted);
    }
}

void AHW_PlayerController::BlockPlayerLocal(AHW_PlayerState* TargetPlayer, bool bBlocked)
{
    if (GetGameInstance() && (UHW_ModerationServiceSubsystem* Moderation = GetGameInstance()->GetSubsystem<UHW_ModerationServiceSubsystem>()))
    {
        Moderation->SetPlayerBlockedLocal(ResolvePlayerId(TargetPlayer), bBlocked);
    }
}

void AHW_PlayerController::ReportPlayerLocal(AHW_PlayerState* TargetPlayer, const FString& Reason)
{
    if (GetGameInstance() && (UHW_ModerationServiceSubsystem* Moderation = GetGameInstance()->GetSubsystem<UHW_ModerationServiceSubsystem>()))
    {
        Moderation->ReportPlayer(ResolvePlayerId(TargetPlayer), Reason);
    }
}

void AHW_PlayerController::SetMuteAllInRoomForHost(bool bMuteAll)
{
    if (!HasAuthority())
    {
        return;
    }

    if (GetGameInstance() && (UHW_ModerationServiceSubsystem* Moderation = GetGameInstance()->GetSubsystem<UHW_ModerationServiceSubsystem>()))
    {
        Moderation->SetMuteAllInRoom(bMuteAll);
    }
}

bool AHW_PlayerController::ShouldShowVoiceUI() const
{
    return IsVoiceEnabledInSettings();
}

void AHW_PlayerController::HandlePushToTalkPressed()
{
    UHW_VoiceServiceSubsystem* Voice = GetGameInstance() ? GetGameInstance()->GetSubsystem<UHW_VoiceServiceSubsystem>() : nullptr;
    AHW_PlayerState* HWPS = GetPlayerState<AHW_PlayerState>();
    if (!Voice || !HWPS || !ShouldShowVoiceUI())
    {
        return;
    }

    Voice->SetPushToTalkActive(true);
    HWPS->ServerSetVoiceTalking(true);
}

void AHW_PlayerController::HandlePushToTalkReleased()
{
    UHW_VoiceServiceSubsystem* Voice = GetGameInstance() ? GetGameInstance()->GetSubsystem<UHW_VoiceServiceSubsystem>() : nullptr;
    AHW_PlayerState* HWPS = GetPlayerState<AHW_PlayerState>();
    if (!Voice || !HWPS)
    {
        return;
    }

    Voice->SetPushToTalkActive(false);
    HWPS->ServerSetVoiceTalking(false);
}

FString AHW_PlayerController::SanitizeChatMessage(const FString& RawMessage) const
{
    FString Sanitized;
    Sanitized.Reserve(RawMessage.Len());

    for (const TCHAR Character : RawMessage)
    {
        if (Character == TEXT('\n') || Character == TEXT('\r') || Character == TEXT('\t'))
        {
            Sanitized.AppendChar(TEXT(' '));
            continue;
        }

        if (!FChar::IsControl(Character))
        {
            Sanitized.AppendChar(Character);
        }
    }

    Sanitized = Sanitized.TrimStartAndEnd();
    Sanitized = Sanitized.Left(256);
    return Sanitized;
}

FString AHW_PlayerController::ResolvePlayerId(const AHW_PlayerState* TargetPlayer) const
{
    if (!TargetPlayer)
    {
        return TEXT("Invalid");
    }

    return TargetPlayer->GetUniqueId().ToString();
}
