#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "HW_PlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDisplayNameChanged);

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

    UPROPERTY(BlueprintAssignable, Category = "Hangout|Player")
    FOnDisplayNameChanged OnDisplayNameChanged;

protected:
    UPROPERTY(ReplicatedUsing = OnRep_DisplayName, BlueprintReadOnly, Category = "Hangout|Player")
    FString DisplayName;

    UFUNCTION()
    void OnRep_DisplayName();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
