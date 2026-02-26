#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HW_Types.h"
#include "HW_PlayerController.generated.h"

class UUserWidget;

UCLASS()
class HANGOUTWORLD_API AHW_PlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category = "Hangout|Chat")
    void SendChatMessage(const FString& Message);

    UFUNCTION(Server, Reliable)
    void ServerSendChatMessage(const FString& Message);

    UFUNCTION(BlueprintCallable, Category = "Hangout|Emote")
    void TriggerEmote(EHWEmoteType Emote);

    UFUNCTION(BlueprintCallable, Category = "Hangout|UI")
    void SetChatFocus(bool bFocused);

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Hangout|UI")
    TSubclassOf<UUserWidget> LobbyWidgetClass;

    UPROPERTY()
    TObjectPtr<UUserWidget> LobbyWidget;

private:
    FString SanitizeChatMessage(const FString& RawMessage) const;
};
