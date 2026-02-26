#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "HW_VoiceServiceInterface.h"
#include "HW_VoiceServiceSubsystem.generated.h"

class UHW_GameInstance;

UCLASS()
class HANGOUTWORLD_API UHW_VoiceServiceSubsystem : public UGameInstanceSubsystem, public IHW_VoiceServiceInterface
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    virtual void InitializeVoice() override;
    virtual void ShutdownVoice() override;
    virtual bool IsVoiceAvailable() const override;
    virtual void SetPushToTalkActive(bool bActive) override;

    UFUNCTION(BlueprintPure, Category = "Hangout|Voice")
    bool IsPushToTalkActive() const { return bPushToTalkActive; }

private:
    UFUNCTION()
    void HandleVoiceToggleChanged(bool bEnabled);

    UPROPERTY()
    bool bVoiceInitialized = false;

    UPROPERTY()
    bool bPushToTalkActive = false;
};
