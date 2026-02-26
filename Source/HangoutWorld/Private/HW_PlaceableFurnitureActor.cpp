#include "HW_PlaceableFurnitureActor.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "Net/UnrealNetwork.h"

AHW_PlaceableFurnitureActor::AHW_PlaceableFurnitureActor()
{
    bReplicates = true;
    SetReplicateMovement(true);

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FurnitureMesh"));
    MeshComponent->SetIsReplicated(false);
    MeshComponent->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
    RootComponent = MeshComponent;
}

void AHW_PlaceableFurnitureActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AHW_PlaceableFurnitureActor, PlacedItemId);
    DOREPLIFETIME(AHW_PlaceableFurnitureActor, CatalogItemId);
    DOREPLIFETIME(AHW_PlaceableFurnitureActor, RoomOwnerUniqueId);
}
