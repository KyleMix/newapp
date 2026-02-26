#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "HW_Types.h"
#include "HW_GameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChatMessagesUpdated, const TArray<FHWChatMessage>&, Messages);

UCLASS()
class HANGOUTWORLD_API AHW_GameState : public AGameStateBase
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category = "Hangout|Chat")
    const TArray<FHWChatMessage>& GetChatMessages() const { return ChatMessages; }

    UFUNCTION(BlueprintCallable, Category = "Hangout|Chat")
    void AddChatMessage(const FString& SenderName, const FString& Message);

    UPROPERTY(BlueprintAssignable, Category = "Hangout|Chat")
    FOnChatMessagesUpdated OnChatMessagesUpdated;

protected:
    UPROPERTY(ReplicatedUsing = OnRep_ChatMessages, BlueprintReadOnly, Category = "Hangout|Chat")
    TArray<FHWChatMessage> ChatMessages;

    UFUNCTION()
    void OnRep_ChatMessages();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
