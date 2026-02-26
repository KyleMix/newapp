#include "HW_GameInstance.h"
#include "Kismet/GameplayStatics.h"

void UHW_GameInstance::HostLobby(const FString& MapPath)
{
    UGameplayStatics::OpenLevel(GetWorld(), FName(*MapPath), true, TEXT("listen"));
}

void UHW_GameInstance::JoinLobby(const FString& Address)
{
    if (APlayerController* PC = GetFirstLocalPlayerController())
    {
        PC->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
    }
}
