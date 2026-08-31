// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Framework/Commands/Commands.h"
#include "UE5AssetTrackerStyle.h"

class FUE5AssetTrackerCommands : public TCommands<FUE5AssetTrackerCommands>
{
public:

	FUE5AssetTrackerCommands()
		: TCommands<FUE5AssetTrackerCommands>(TEXT("UE5AssetTracker"), NSLOCTEXT("Contexts", "UE5AssetTracker", "UE5AssetTracker Plugin"), NAME_None, FUE5AssetTrackerStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};