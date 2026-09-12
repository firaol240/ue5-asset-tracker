#pragma once


struct FAssetGroup {
    FString Name;
    FString Author;
    FString License;

    TArray<FAssetData> Assets;
};