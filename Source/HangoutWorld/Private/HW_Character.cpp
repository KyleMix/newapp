#include "HW_Character.h"

#include "Components/TextRenderComponent.h"
#include "Engine/Engine.h"
#include "HW_PlayerState.h"

AHW_Character::AHW_Character()
{
    bReplicates = true;

    NameText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("NameText"));
    NameText->SetupAttachment(GetRootComponent());
    NameText->SetRelativeLocation(FVector(0.f, 0.f, 110.f));
    NameText->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
    NameText->SetTextRenderColor(FColor::White);
    NameText->SetWorldSize(30.f);

    TalkingText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TalkingText"));
    TalkingText->SetupAttachment(GetRootComponent());
    TalkingText->SetRelativeLocation(FVector(0.f, 0.f, 140.f));
    TalkingText->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
    TalkingText->SetText(FText::FromString(TEXT("🎤")));
    TalkingText->SetTextRenderColor(FColor::Green);
    TalkingText->SetWorldSize(20.f);
    TalkingText->SetVisibility(false);
}

void AHW_Character::BeginPlay()
{
    Super::BeginPlay();
    BindPlayerStateEvents();
    RefreshNameplate();
    if (const AHW_PlayerState* HWPS = GetPlayerState<AHW_PlayerState>())
    {
        RefreshTalkingIndicator(HWPS->IsVoiceTalking());
    }
}

void AHW_Character::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();
    BindPlayerStateEvents();
    RefreshNameplate();
    if (const AHW_PlayerState* HWPS = GetPlayerState<AHW_PlayerState>())
    {
        RefreshTalkingIndicator(HWPS->IsVoiceTalking());
    }
}

void AHW_Character::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UnbindPlayerStateEvents();
    Super::EndPlay(EndPlayReason);
}

void AHW_Character::BindPlayerStateEvents()
{
    UnbindPlayerStateEvents();

    if (AHW_PlayerState* HWPS = GetPlayerState<AHW_PlayerState>())
    {
        HWPS->OnDisplayNameChanged.AddDynamic(this, &AHW_Character::RefreshNameplate);
        HWPS->OnVoiceTalkingChanged.AddDynamic(this, &AHW_Character::RefreshTalkingIndicator);
        BoundPlayerState = HWPS;
    }
}

void AHW_Character::UnbindPlayerStateEvents()
{
    if (BoundPlayerState.IsValid())
    {
        BoundPlayerState->OnDisplayNameChanged.RemoveDynamic(this, &AHW_Character::RefreshNameplate);
        BoundPlayerState->OnVoiceTalkingChanged.RemoveDynamic(this, &AHW_Character::RefreshTalkingIndicator);
        BoundPlayerState = nullptr;
    }
}

void AHW_Character::RefreshNameplate()
{
    if (AHW_PlayerState* HWPS = GetPlayerState<AHW_PlayerState>())
    {
        NameText->SetText(FText::FromString(HWPS->GetDisplayName()));
    }
}

void AHW_Character::RefreshTalkingIndicator(bool bTalking)
{
    TalkingText->SetVisibility(bTalking);
}

void AHW_Character::ServerPlayEmote_Implementation(EHWEmoteType Emote)
{
    MulticastPlayEmote(Emote);
}

void AHW_Character::MulticastPlayEmote_Implementation(EHWEmoteType Emote)
{
    UAnimMontage* MontageToPlay = nullptr;
    if (Emote == EHWEmoteType::Wave)
    {
        MontageToPlay = WaveMontage;
    }
    else if (Emote == EHWEmoteType::Point)
    {
        MontageToPlay = PointMontage;
    }

    if (MontageToPlay)
    {
        PlayAnimMontage(MontageToPlay);
    }
    else if (GEngine)
    {
        const FString EmoteLabel = Emote == EHWEmoteType::Wave ? TEXT("Wave") : TEXT("Point");
        GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("%s emote"), *EmoteLabel));
    }

    OnEmotePlayed(Emote);
}
