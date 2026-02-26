#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "HW_PlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDisplayNameChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVoiceTalkingChanged, bool, bIsTalking);

UCLASS()
class HANGOUTWORLD_API AHW_PlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    AHW_PlayerState();

    UFUNCTION(BlueprintCallable, Category = "Hangout|Player")
    const FString& GetDisplayName() const { return DisplayName; }

    UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Hangout|Player")
    void ServerSetDisplayName(const FString& NewName);

    UFUNCTION(BlueprintCallable, Category = "Hangout|Chat")
    bool CanSendChatMessage(float WindowSeconds = 5.f, int32 MaxMessagesInWindow = 5);

    UFUNCTION(Server, Unreliable)
    void ServerSetVoiceTalking(bool bTalking);

    UFUNCTION(BlueprintPure, Category = "Hangout|Voice")
    bool IsVoiceTalking() const { return bIsVoiceTalking; }

    UPROPERTY(BlueprintAssignable, Category = "Hangout|Player")
    FOnDisplayNameChanged OnDisplayNameChanged;

    UPROPERTY(BlueprintAssignable, Category = "Hangout|Voice")
    FOnVoiceTalkingChanged OnVoiceTalkingChanged;

protected:
    UPROPERTY(ReplicatedUsing = OnRep_DisplayName, BlueprintReadOnly, Category = "Hangout|Player")
    FString DisplayName;

    UPROPERTY(ReplicatedUsing = OnRep_IsVoiceTalking, BlueprintReadOnly, Category = "Hangout|Voice")
    bool bIsVoiceTalking = false;

    UPROPERTY()
    TArray<float> RecentChatServerTimes;

    UFUNCTION()
    void OnRep_DisplayName();

    UFUNCTION()
    void OnRep_IsVoiceTalking();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    float LastVoiceStateServerChangeTime = -1000.f;
};
