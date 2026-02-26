#include "HW_RoomJsonUtils.h"

#include "Dom/JsonObject.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Policies/PrettyJsonPrintPolicy.h"
#include "Serialization/JsonSerializer.h"

FString FHW_RoomJsonUtils::GetLayoutFilePath(const FString& OwnerUniqueId)
{
    const FString SanitizedOwner = OwnerUniqueId.IsEmpty() ? TEXT("Unknown") : OwnerUniqueId.Replace(TEXT("/"), TEXT("_"));
    const FString Directory = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("HangoutWorld/Rooms"));
    return FPaths::Combine(Directory, FString::Printf(TEXT("%s.json"), *SanitizedOwner));
}

bool FHW_RoomJsonUtils::SaveLayout(const FString& OwnerUniqueId, const TArray<FHWPlacedFurnitureRecord>& Records)
{
    const FString FilePath = GetLayoutFilePath(OwnerUniqueId);
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(FilePath), true);

    TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();
    Root->SetStringField(TEXT("ownerId"), OwnerUniqueId);

    TArray<TSharedPtr<FJsonValue>> Items;
    for (const FHWPlacedFurnitureRecord& Item : Records)
    {
        TSharedRef<FJsonObject> ItemObject = MakeShared<FJsonObject>();
        ItemObject->SetStringField(TEXT("itemId"), Item.ItemId.ToString(EGuidFormats::DigitsWithHyphensLower));
        ItemObject->SetStringField(TEXT("catalogId"), Item.CatalogId.ToString());

        TSharedRef<FJsonObject> TransformObject = MakeShared<FJsonObject>();
        const FVector Location = Item.Transform.GetLocation();
        const FRotator Rotation = Item.Transform.GetRotation().Rotator();
        const FVector Scale = Item.Transform.GetScale3D();

        TransformObject->SetNumberField(TEXT("lx"), Location.X);
        TransformObject->SetNumberField(TEXT("ly"), Location.Y);
        TransformObject->SetNumberField(TEXT("lz"), Location.Z);
        TransformObject->SetNumberField(TEXT("rx"), Rotation.Roll);
        TransformObject->SetNumberField(TEXT("ry"), Rotation.Pitch);
        TransformObject->SetNumberField(TEXT("rz"), Rotation.Yaw);
        TransformObject->SetNumberField(TEXT("sx"), Scale.X);
        TransformObject->SetNumberField(TEXT("sy"), Scale.Y);
        TransformObject->SetNumberField(TEXT("sz"), Scale.Z);

        ItemObject->SetObjectField(TEXT("transform"), TransformObject);
        Items.Add(MakeShared<FJsonValueObject>(ItemObject));
    }

    Root->SetArrayField(TEXT("items"), Items);

    FString Payload;
    const TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&Payload);
    if (!FJsonSerializer::Serialize(Root, Writer))
    {
        return false;
    }

    return FFileHelper::SaveStringToFile(Payload, *FilePath);
}

bool FHW_RoomJsonUtils::LoadLayout(const FString& OwnerUniqueId, TArray<FHWPlacedFurnitureRecord>& OutRecords)
{
    OutRecords.Reset();

    const FString FilePath = GetLayoutFilePath(OwnerUniqueId);
    if (!FPaths::FileExists(FilePath))
    {
        return true;
    }

    FString Payload;
    if (!FFileHelper::LoadFileToString(Payload, *FilePath))
    {
        UE_LOG(LogTemp, Warning, TEXT("Room layout read failed for owner '%s'"), *OwnerUniqueId);
        return false;
    }

    TSharedPtr<FJsonObject> Root;
    const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Payload);
    if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("Room layout JSON invalid for owner '%s'. Starting empty."), *OwnerUniqueId);
        return false;
    }

    const TArray<TSharedPtr<FJsonValue>>* Items = nullptr;
    if (!Root->TryGetArrayField(TEXT("items"), Items) || !Items)
    {
        return true;
    }

    for (const TSharedPtr<FJsonValue>& JsonValue : *Items)
    {
        const TSharedPtr<FJsonObject> ItemObject = JsonValue ? JsonValue->AsObject() : nullptr;
        if (!ItemObject.IsValid())
        {
            continue;
        }

        FHWPlacedFurnitureRecord& Record = OutRecords.AddDefaulted_GetRef();
        FGuid::Parse(ItemObject->GetStringField(TEXT("itemId")), Record.ItemId);
        Record.CatalogId = FName(ItemObject->GetStringField(TEXT("catalogId")));

        const TSharedPtr<FJsonObject>* TransformObject = nullptr;
        if (ItemObject->TryGetObjectField(TEXT("transform"), TransformObject) && TransformObject)
        {
            FVector Location;
            FRotator Rotation;
            FVector Scale(1.f);

            Location.X = (*TransformObject)->GetNumberField(TEXT("lx"));
            Location.Y = (*TransformObject)->GetNumberField(TEXT("ly"));
            Location.Z = (*TransformObject)->GetNumberField(TEXT("lz"));
            Rotation.Roll = (*TransformObject)->GetNumberField(TEXT("rx"));
            Rotation.Pitch = (*TransformObject)->GetNumberField(TEXT("ry"));
            Rotation.Yaw = (*TransformObject)->GetNumberField(TEXT("rz"));
            Scale.X = (*TransformObject)->GetNumberField(TEXT("sx"));
            Scale.Y = (*TransformObject)->GetNumberField(TEXT("sy"));
            Scale.Z = (*TransformObject)->GetNumberField(TEXT("sz"));

            Record.Transform = FTransform(Rotation, Location, Scale);
        }
    }

    return true;
}
