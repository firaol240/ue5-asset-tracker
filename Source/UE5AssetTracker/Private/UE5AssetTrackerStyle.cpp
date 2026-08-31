// Copyright Epic Games, Inc. All Rights Reserved.

#include "UE5AssetTrackerStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FUE5AssetTrackerStyle::StyleInstance = nullptr;

void FUE5AssetTrackerStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FUE5AssetTrackerStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FUE5AssetTrackerStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("UE5AssetTrackerStyle"));
	return StyleSetName;
}

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FUE5AssetTrackerStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("UE5AssetTrackerStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("UE5AssetTracker")->GetBaseDir() / TEXT("Resources"));

	Style->Set("UE5AssetTracker.OpenPluginWindow", new IMAGE_BRUSH_SVG(TEXT("PlaceholderButtonIcon"), Icon20x20));

	return Style;
}

void FUE5AssetTrackerStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FUE5AssetTrackerStyle::Get()
{
	return *StyleInstance;
}
