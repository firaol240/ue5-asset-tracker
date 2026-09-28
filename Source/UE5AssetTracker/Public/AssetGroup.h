#pragma once

#include "CoreMinimal.h"
#include "AssetRegistry/AssetData.h"

struct FAssetGroup {
    FString GroupName;
    FString Author;
    FString License;

    TArray<FAssetData> Assets;
};