#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "HW_GameInstance.generated.h"

UCLASS()
class HANGOUTWORLD_API UHW_GameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Hangout|Session")
    FString LocalPlayerName = TEXT("Player");

    UFUNCTION(BlueprintCallable, Category = "Hangout|Session")
    void HostLobby(const FString& MapPath = TEXT("/Game/HangoutWorld/Maps/HangoutLobby"));

    UFUNCTION(BlueprintCallable, Category = "Hangout|Session")
    void JoinLobby(const FString& Address);
};
