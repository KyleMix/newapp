#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HW_PlaceableFurnitureActor.generated.h"

class UStaticMeshComponent;

UCLASS()
class HANGOUTWORLD_API AHW_PlaceableFurnitureActor : public AActor
{
    GENERATED_BODY()

public:
    AHW_PlaceableFurnitureActor();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hangout|Rooms")
    TObjectPtr<UStaticMeshComponent> MeshComponent;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Hangout|Rooms")
    FGuid PlacedItemId;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Hangout|Rooms")
    FName CatalogItemId;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Hangout|Rooms")
    FString RoomOwnerUniqueId;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
