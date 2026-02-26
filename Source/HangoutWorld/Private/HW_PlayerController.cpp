#include "HW_PlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "HW_Character.h"
#include "HW_GameInstance.h"
#include "HW_GameState.h"
#include "HW_PlayerState.h"

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
