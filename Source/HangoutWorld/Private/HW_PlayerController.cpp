#include "HW_PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Character.h"
#include "HW_Character.h"
#include "HW_GameInstance.h"
#include "HW_GameState.h"
#include "HW_PlayerState.h"

void AHW_PlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (IsLocalController() && LobbyWidgetClass)
    {
        LobbyWidget = CreateWidget(this, LobbyWidgetClass);
        if (LobbyWidget)
        {
            LobbyWidget->AddToViewport();
        }
    }

    if (UHW_GameInstance* GI = GetGameInstance<UHW_GameInstance>())
    {
        if (AHW_PlayerState* HWPS = GetPlayerState<AHW_PlayerState>())
        {
            HWPS->ServerSetDisplayName(GI->LocalPlayerName);
        }
    }
}

void AHW_PlayerController::SendChatMessage(const FString& Message)
{
    if (!Message.TrimStartAndEnd().IsEmpty())
    {
        ServerSendChatMessage(Message);
    }
}

void AHW_PlayerController::ServerSendChatMessage_Implementation(const FString& Message)
{
    if (AHW_GameState* HWS = GetWorld()->GetGameState<AHW_GameState>())
    {
        const FString Sender = GetPlayerState<AHW_PlayerState>() ? GetPlayerState<AHW_PlayerState>()->GetDisplayName() : TEXT("Player");
        HWS->AddChatMessage(Sender, Message);
    }
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
