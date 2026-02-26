#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HW_Types.h"
#include "HW_Character.generated.h"

class UTextRenderComponent;
class UAnimMontage;
class AHW_PlayerState;

UCLASS()
class HANGOUTWORLD_API AHW_Character : public ACharacter
{
    GENERATED_BODY()

public:
    AHW_Character();

    UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Hangout|Emote")
    void ServerPlayEmote(EHWEmoteType Emote);

    UFUNCTION(NetMulticast, Unreliable)
    void MulticastPlayEmote(EHWEmoteType Emote);

    UFUNCTION(BlueprintImplementableEvent, Category = "Hangout|Emote")
    void OnEmotePlayed(EHWEmoteType Emote);

protected:
    virtual void BeginPlay() override;
    virtual void OnRep_PlayerState() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hangout|UI")
    TObjectPtr<UTextRenderComponent> NameText;

    UPROPERTY(EditDefaultsOnly, Category = "Hangout|Emote")
    TObjectPtr<UAnimMontage> WaveMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Hangout|Emote")
    TObjectPtr<UAnimMontage> PointMontage;

private:
    UFUNCTION()
    void RefreshNameplate();

    void BindPlayerStateEvents();
    void UnbindPlayerStateEvents();

    TWeakObjectPtr<AHW_PlayerState> BoundPlayerState;
};
