#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSessionSettings.h"
#include "HW_GameInstance.generated.h"

USTRUCT(BlueprintType)
struct FHWSessionSearchResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Hangout|Session")
    FString SessionId;

    UPROPERTY(BlueprintReadOnly, Category = "Hangout|Session")
    FString HostName;

    UPROPERTY(BlueprintReadOnly, Category = "Hangout|Session")
    int32 CurrentPlayers = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Hangout|Session")
    int32 MaxPlayers = 0;

    FOnlineSessionSearchResult NativeResult;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionSearchCompleted, const TArray<FHWSessionSearchResult>&, Results);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionActionCompleted, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVoiceChatEnabledChanged, bool, bEnabled);

UCLASS()
class HANGOUTWORLD_API UHW_GameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    virtual void Init() override;
    virtual void Shutdown() override;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Hangout|Session")
    FString LocalPlayerName = TEXT("Player");

    UPROPERTY(Config, BlueprintReadOnly, EditDefaultsOnly, Category = "Hangout|Voice")
    bool bVoiceChatEnabledByDefault = false;

    UFUNCTION(BlueprintPure, Category = "Hangout|Voice")
    bool IsVoiceChatEnabled() const { return bVoiceChatEnabledRuntime; }

    UFUNCTION(BlueprintCallable, Category = "Hangout|Voice")
    void SetVoiceChatEnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "Hangout|Session")
    void HostLobby(int32 MaxPublicConnections = 8, const FString& MapPath = TEXT("/Game/HangoutWorld/Maps/HangoutLobby"));

    UFUNCTION(BlueprintCallable, Category = "Hangout|Session")
    void FindLobbySessions(int32 MaxResults = 20);

    UFUNCTION(BlueprintCallable, Category = "Hangout|Session")
    void JoinLobbySession(int32 SessionIndex);

    UPROPERTY(BlueprintAssignable, Category = "Hangout|Session")
    FOnSessionSearchCompleted OnSessionSearchCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Hangout|Session")
    FOnSessionActionCompleted OnHostCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Hangout|Session")
    FOnSessionActionCompleted OnJoinCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Hangout|Voice")
    FOnVoiceChatEnabledChanged OnVoiceChatEnabledChanged;


    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hangout|Rooms")
    FString RoomMapPath = TEXT("/Game/HangoutWorld/Maps/HangoutRoom");

    UFUNCTION(BlueprintCallable, Category = "Hangout|Rooms")
    void HostMyRoom(int32 MaxPublicConnections = 8);

    UFUNCTION(BlueprintCallable, Category = "Hangout|Rooms")
    void VisitFriendRoom(int32 SessionIndex);

protected:
    FString PendingTravelMapPath;
    int32 PendingMaxPublicConnections = 8;

    TArray<FHWSessionSearchResult> CachedSearchResults;

    TSharedPtr<FOnlineSessionSettings> HostSessionSettings;
    TSharedPtr<FOnlineSessionSearch> SessionSearch;

    IOnlineSessionPtr GetSessionInterface() const;

    void StartCreateSession(int32 MaxPublicConnections);

    void HandleCreateSessionComplete(FName SessionName, bool bWasSuccessful);
    void HandleDestroySessionComplete(FName SessionName, bool bWasSuccessful);
    void HandleFindSessionsComplete(bool bWasSuccessful);
    void HandleJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

    FDelegateHandle CreateSessionCompleteHandle;
    FDelegateHandle DestroySessionCompleteHandle;
    FDelegateHandle FindSessionsCompleteHandle;
    FDelegateHandle JoinSessionCompleteHandle;

private:
    bool bVoiceChatEnabledRuntime = false;
};
