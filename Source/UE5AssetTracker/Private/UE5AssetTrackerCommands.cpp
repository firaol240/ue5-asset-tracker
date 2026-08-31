// Copyright Epic Games, Inc. All Rights Reserved.

#include "UE5AssetTrackerCommands.h"

#define LOCTEXT_NAMESPACE "FUE5AssetTrackerModule"

void FUE5AssetTrackerCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "UE5AssetTracker", "Bring up UE5AssetTracker window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
