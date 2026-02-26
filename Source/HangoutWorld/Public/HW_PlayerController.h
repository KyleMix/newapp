#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HW_Types.h"
#include "HW_PlayerController.generated.h"

class AHW_PlayerState;
class UUserWidget;

class AHW_PlaceableFurnitureActor;
class AHW_RoomManager;

UCLASS()
class HANGOUTWORLD_API AHW_PlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

    UFUNCTION(BlueprintCallable, Category = "Hangout|Chat")
    void SendChatMessage(const FString& Message);

    UFUNCTION(Server, Reliable)
    void ServerSendChatMessage(const FString& Message);

    UFUNCTION(BlueprintCallable, Category = "Hangout|Emote")
    void TriggerEmote(EHWEmoteType Emote);

    UFUNCTION(BlueprintCallable, Category = "Hangout|UI")
    void SetChatFocus(bool bFocused);

    UFUNCTION(BlueprintCallable, Category = "Hangout|Voice")
    void SetVoiceEnabledInSettings(bool bEnabled);

    UFUNCTION(BlueprintPure, Category = "Hangout|Voice")
    bool IsVoiceEnabledInSettings() const;

    UFUNCTION(BlueprintCallable, Category = "Hangout|Moderation")
    void MutePlayerLocal(AHW_PlayerState* TargetPlayer, bool bMuted);

    UFUNCTION(BlueprintCallable, Category = "Hangout|Moderation")
    void BlockPlayerLocal(AHW_PlayerState* TargetPlayer, bool bBlocked);

    UFUNCTION(BlueprintCallable, Category = "Hangout|Moderation")
    void ReportPlayerLocal(AHW_PlayerState* TargetPlayer, const FString& Reason);

    UFUNCTION(BlueprintCallable, Category = "Hangout|Moderation")
    void SetMuteAllInRoomForHost(bool bMuteAll);

    UFUNCTION(BlueprintPure, Category = "Hangout|Voice")
    bool ShouldShowVoiceUI() const;


    UFUNCTION(BlueprintCallable, Category = "Hangout|Rooms")
    void SetRoomEditModeEnabled(bool bEnabled);

    UFUNCTION(BlueprintPure, Category = "Hangout|Rooms")
    bool IsRoomEditModeEnabled() const { return bRoomEditModeEnabled; }

    UFUNCTION(BlueprintCallable, Category = "Hangout|Rooms")
    void SetRoomGridSnapEnabled(bool bEnabled);

    UFUNCTION(BlueprintPure, Category = "Hangout|Rooms")
    bool IsRoomGridSnapEnabled() const { return bRoomGridSnapEnabled; }

    UFUNCTION(BlueprintCallable, Category = "Hangout|Rooms")
    void SetSelectedFurnitureCatalogId(FName CatalogId) { SelectedFurnitureCatalogId = CatalogId; }

    UFUNCTION(BlueprintPure, Category = "Hangout|Rooms")
    FName GetSelectedFurnitureCatalogId() const { return SelectedFurnitureCatalogId; }

    UFUNCTION(BlueprintCallable, Category = "Hangout|Rooms")
    void ConfirmFurniturePlacement(const FTransform& DesiredTransform);

    UFUNCTION(Server, Reliable)
    void ServerConfirmFurniturePlacement(FName CatalogId, FTransform DesiredTransform);

    UFUNCTION(BlueprintCallable, Category = "Hangout|Rooms")
    void DeleteFurnitureItem(AHW_PlaceableFurnitureActor* FurnitureActor);

    UFUNCTION(Server, Reliable)
    void ServerDeleteFurnitureItem(const FGuid& ItemId);

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Hangout|UI")
    TSubclassOf<UUserWidget> LobbyWidgetClass;

    UPROPERTY()
    TObjectPtr<UUserWidget> LobbyWidget;

private:
    void HandlePushToTalkPressed();
    void HandlePushToTalkReleased();

    FString SanitizeChatMessage(const FString& RawMessage) const;
    FString ResolvePlayerId(const AHW_PlayerState* TargetPlayer) const;

    UPROPERTY(Transient)
    bool bRoomEditModeEnabled = false;

    UPROPERTY(Transient)
    bool bRoomGridSnapEnabled = true;

    UPROPERTY(Transient)
    FName SelectedFurnitureCatalogId = NAME_None;

    AHW_RoomManager* FindRoomManager() const;
};
