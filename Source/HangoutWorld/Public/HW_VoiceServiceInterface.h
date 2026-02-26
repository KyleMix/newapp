#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HW_VoiceServiceInterface.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UHW_VoiceServiceInterface : public UInterface
{
    GENERATED_BODY()
};

class HANGOUTWORLD_API IHW_VoiceServiceInterface
{
    GENERATED_BODY()

public:
    virtual void InitializeVoice() = 0;
    virtual void ShutdownVoice() = 0;
    virtual bool IsVoiceAvailable() const = 0;
    virtual void SetPushToTalkActive(bool bActive) = 0;
};
