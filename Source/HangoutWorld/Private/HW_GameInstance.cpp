#include "HW_GameInstance.h"

#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystem.h"
#include "Engine/LocalPlayer.h"

namespace
{
    const FName HWSessionName = NAME_GameSession;
}

DEFINE_LOG_CATEGORY_STATIC(LogHWSession, Log, All);

void UHW_GameInstance::Init()
{
    Super::Init();
    bVoiceChatEnabledRuntime = bVoiceChatEnabledByDefault;
}

void UHW_GameInstance::SetVoiceChatEnabled(bool bEnabled)
{
    if (bVoiceChatEnabledRuntime == bEnabled)
    {
        return;
    }

    bVoiceChatEnabledRuntime = bEnabled;
    OnVoiceChatEnabledChanged.Broadcast(bVoiceChatEnabledRuntime);
}

void UHW_GameInstance::Shutdown()
{
    if (IOnlineSessionPtr SessionInterface = GetSessionInterface())
    {
        if (CreateSessionCompleteHandle.IsValid())
        {
            SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteHandle);
        }

        if (DestroySessionCompleteHandle.IsValid())
        {
            SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteHandle);
        }

        if (FindSessionsCompleteHandle.IsValid())
        {
            SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteHandle);
        }

        if (JoinSessionCompleteHandle.IsValid())
        {
            SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteHandle);
        }
    }

    Super::Shutdown();
}

IOnlineSessionPtr UHW_GameInstance::GetSessionInterface() const
{
    if (const IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get())
    {
        return OnlineSubsystem->GetSessionInterface();
    }

    return nullptr;
}

void UHW_GameInstance::HostLobby(int32 MaxPublicConnections, const FString& MapPath)
{
    if (MapPath.IsEmpty())
    {
        UE_LOG(LogHWSession, Warning, TEXT("HostLobby rejected: map path is empty."));
        OnHostCompleted.Broadcast(false);
        return;
    }

    PendingTravelMapPath = MapPath;
    PendingMaxPublicConnections = FMath::Max(2, MaxPublicConnections);

    IOnlineSessionPtr SessionInterface = GetSessionInterface();
    if (!SessionInterface.IsValid())
    {
        UE_LOG(LogHWSession, Warning, TEXT("HostLobby failed: online session interface is invalid."));
        OnHostCompleted.Broadcast(false);
        return;
    }

    if (SessionInterface->GetNamedSession(HWSessionName) != nullptr)
    {
        if (DestroySessionCompleteHandle.IsValid())
        {
            SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteHandle);
        }

        DestroySessionCompleteHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(
            FOnDestroySessionCompleteDelegate::CreateUObject(this, &UHW_GameInstance::HandleDestroySessionComplete));

        SessionInterface->DestroySession(HWSessionName);
        return;
    }

    StartCreateSession(PendingMaxPublicConnections);
}

void UHW_GameInstance::StartCreateSession(int32 MaxPublicConnections)
{
    IOnlineSessionPtr SessionInterface = GetSessionInterface();
    if (!SessionInterface.IsValid())
    {
        OnHostCompleted.Broadcast(false);
        return;
    }

    HostSessionSettings = MakeShared<FOnlineSessionSettings>();
    HostSessionSettings->bIsLANMatch = true;
    HostSessionSettings->NumPublicConnections = FMath::Max(2, MaxPublicConnections);
    HostSessionSettings->bShouldAdvertise = true;
    HostSessionSettings->bAllowJoinInProgress = true;
    HostSessionSettings->bAllowJoinViaPresence = false;
    HostSessionSettings->bUsesPresence = false;
    HostSessionSettings->bUseLobbiesIfAvailable = false;

    if (CreateSessionCompleteHandle.IsValid())
    {
        SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteHandle);
    }

    CreateSessionCompleteHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(
        FOnCreateSessionCompleteDelegate::CreateUObject(this, &UHW_GameInstance::HandleCreateSessionComplete));

    const ULocalPlayer* LocalPlayer = GetFirstGamePlayer();
    const bool bCreateRequestSent = LocalPlayer
        ? SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), HWSessionName, *HostSessionSettings)
        : false;

    if (!bCreateRequestSent)
    {
        SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteHandle);
        CreateSessionCompleteHandle.Reset();
        OnHostCompleted.Broadcast(false);
    }
}

void UHW_GameInstance::FindLobbySessions(int32 MaxResults)
{
    IOnlineSessionPtr SessionInterface = GetSessionInterface();
    if (!SessionInterface.IsValid())
    {
        UE_LOG(LogHWSession, Warning, TEXT("FindLobbySessions failed: online session interface is invalid."));
        OnSessionSearchCompleted.Broadcast(TArray<FHWSessionSearchResult>());
        return;
    }

    SessionSearch = MakeShared<FOnlineSessionSearch>();
    SessionSearch->bIsLanQuery = true;
    SessionSearch->MaxSearchResults = FMath::Max(1, MaxResults);

    if (FindSessionsCompleteHandle.IsValid())
    {
        SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteHandle);
    }

    FindSessionsCompleteHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(
        FOnFindSessionsCompleteDelegate::CreateUObject(this, &UHW_GameInstance::HandleFindSessionsComplete));

    const ULocalPlayer* LocalPlayer = GetFirstGamePlayer();
    const bool bFindRequestSent = LocalPlayer
        ? SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef())
        : false;

    if (!bFindRequestSent)
    {
        SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteHandle);
        FindSessionsCompleteHandle.Reset();
        OnSessionSearchCompleted.Broadcast(TArray<FHWSessionSearchResult>());
    }
}

void UHW_GameInstance::JoinLobbySession(int32 SessionIndex)
{
    IOnlineSessionPtr SessionInterface = GetSessionInterface();
    if (!SessionInterface.IsValid() || !SessionSearch.IsValid() || !SessionSearch->SearchResults.IsValidIndex(SessionIndex))
    {
        UE_LOG(LogHWSession, Warning, TEXT("JoinLobbySession failed: invalid session interface/search/index. Index=%d"), SessionIndex);
        OnJoinCompleted.Broadcast(false);
        return;
    }

    if (JoinSessionCompleteHandle.IsValid())
    {
        SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteHandle);
    }

    JoinSessionCompleteHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(
        FOnJoinSessionCompleteDelegate::CreateUObject(this, &UHW_GameInstance::HandleJoinSessionComplete));

    const ULocalPlayer* LocalPlayer = GetFirstGamePlayer();
    const bool bJoinRequestSent = LocalPlayer
        ? SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), HWSessionName, SessionSearch->SearchResults[SessionIndex])
        : false;

    if (!bJoinRequestSent)
    {
        SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteHandle);
        JoinSessionCompleteHandle.Reset();
        OnJoinCompleted.Broadcast(false);
    }
}


void UHW_GameInstance::HostMyRoom(int32 MaxPublicConnections)
{
    FString OwnerId = LocalPlayerName;
    if (const ULocalPlayer* LocalPlayer = GetFirstGamePlayer())
    {
        if (LocalPlayer->GetPreferredUniqueNetId().IsValid())
        {
            OwnerId = LocalPlayer->GetPreferredUniqueNetId()->ToString();
        }
    }

    OwnerId = OwnerId.Replace(TEXT(" "), TEXT("_"));
    const FString RoomTravel = FString::Printf(TEXT("%s?OwnerId=%s"), *RoomMapPath, *OwnerId);
    UE_LOG(LogHWSession, Log, TEXT("Hosting room for owner '%s' via '%s'"), *OwnerId, *RoomTravel);
    HostLobby(MaxPublicConnections, RoomTravel);
}

void UHW_GameInstance::VisitFriendRoom(int32 SessionIndex)
{
    JoinLobbySession(SessionIndex);
}

void UHW_GameInstance::HandleCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    IOnlineSessionPtr SessionInterface = GetSessionInterface();
    if (SessionInterface.IsValid() && CreateSessionCompleteHandle.IsValid())
    {
        SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteHandle);
        CreateSessionCompleteHandle.Reset();
    }

    if (!bWasSuccessful || SessionName != HWSessionName)
    {
        UE_LOG(LogHWSession, Warning, TEXT("CreateSession failed for '%s'"), *SessionName.ToString());
        OnHostCompleted.Broadcast(false);
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogHWSession, Error, TEXT("CreateSession succeeded but world is null; cannot travel."));
        OnHostCompleted.Broadcast(false);
        return;
    }

    const FString TravelURL = FString::Printf(TEXT("%s?listen"), *PendingTravelMapPath);
    if (!World->ServerTravel(TravelURL))
    {
        UE_LOG(LogHWSession, Warning, TEXT("ServerTravel failed for URL '%s'"), *TravelURL);
        OnHostCompleted.Broadcast(false);
        return;
    }

    OnHostCompleted.Broadcast(true);
}

void UHW_GameInstance::HandleDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
    IOnlineSessionPtr SessionInterface = GetSessionInterface();
    if (SessionInterface.IsValid() && DestroySessionCompleteHandle.IsValid())
    {
        SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteHandle);
        DestroySessionCompleteHandle.Reset();
    }

    if (!bWasSuccessful || SessionName != HWSessionName)
    {
        UE_LOG(LogHWSession, Warning, TEXT("DestroySession failed for '%s'"), *SessionName.ToString());
        OnHostCompleted.Broadcast(false);
        return;
    }

    StartCreateSession(PendingMaxPublicConnections);
}

void UHW_GameInstance::HandleFindSessionsComplete(bool bWasSuccessful)
{
    IOnlineSessionPtr SessionInterface = GetSessionInterface();
    if (SessionInterface.IsValid() && FindSessionsCompleteHandle.IsValid())
    {
        SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteHandle);
        FindSessionsCompleteHandle.Reset();
    }

    CachedSearchResults.Reset();

    if (bWasSuccessful && SessionSearch.IsValid())
    {
        for (const FOnlineSessionSearchResult& NativeResult : SessionSearch->SearchResults)
        {
            FHWSessionSearchResult& OutResult = CachedSearchResults.AddDefaulted_GetRef();
            OutResult.NativeResult = NativeResult;
            OutResult.SessionId = NativeResult.GetSessionIdStr();
            OutResult.HostName = NativeResult.Session.OwningUserName;
            OutResult.MaxPlayers = NativeResult.Session.SessionSettings.NumPublicConnections;
            OutResult.CurrentPlayers = OutResult.MaxPlayers - NativeResult.Session.NumOpenPublicConnections;
        }
    }

    OnSessionSearchCompleted.Broadcast(CachedSearchResults);
}

void UHW_GameInstance::HandleJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    IOnlineSessionPtr SessionInterface = GetSessionInterface();
    if (SessionInterface.IsValid() && JoinSessionCompleteHandle.IsValid())
    {
        SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteHandle);
        JoinSessionCompleteHandle.Reset();
    }

    if (!SessionInterface.IsValid() || SessionName != HWSessionName || Result != EOnJoinSessionCompleteResult::Success)
    {
        UE_LOG(LogHWSession, Warning, TEXT("JoinSession failed for '%s' with result %d"), *SessionName.ToString(), static_cast<int32>(Result));
        OnJoinCompleted.Broadcast(false);
        return;
    }

    FString ResolvedConnectString;
    if (!SessionInterface->GetResolvedConnectString(HWSessionName, ResolvedConnectString))
    {
        OnJoinCompleted.Broadcast(false);
        return;
    }

    if (APlayerController* PlayerController = GetFirstLocalPlayerController())
    {
        PlayerController->ClientTravel(ResolvedConnectString, TRAVEL_Absolute);
        OnJoinCompleted.Broadcast(true);
        return;
    }

    OnJoinCompleted.Broadcast(false);
}
