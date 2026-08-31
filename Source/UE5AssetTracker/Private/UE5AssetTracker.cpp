// Copyright Epic Games, Inc. All Rights Reserved.

#include "../Public/UE5AssetTracker.h"
#include "UE5AssetTrackerStyle.h"
#include "UE5AssetTrackerCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SVerticalBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "ToolMenus.h"

static const FName UE5AssetTrackerTabName("UE5AssetTracker");

#define LOCTEXT_NAMESPACE "FUE5AssetTrackerModule"

void FUE5AssetTrackerModule::StartupModule() {
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FUE5AssetTrackerStyle::Initialize();
	FUE5AssetTrackerStyle::ReloadTextures();

	FUE5AssetTrackerCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FUE5AssetTrackerCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FUE5AssetTrackerModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FUE5AssetTrackerModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(UE5AssetTrackerTabName, FOnSpawnTab::CreateRaw(this, &FUE5AssetTrackerModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FUE5AssetTrackerTabTitle", "UE5AssetTracker"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FUE5AssetTrackerModule::ShutdownModule() {
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FUE5AssetTrackerStyle::Shutdown();

	FUE5AssetTrackerCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(UE5AssetTrackerTabName);
}

TSharedRef<SDockTab> FUE5AssetTrackerModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs) {
	TSharedRef<SVerticalBox> ContentBox = SNew(SVerticalBox);

	ContentBox->AddSlot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)

        + SHorizontalBox::Slot()
        .FillWidth(1.0f)
        [
            SNew(STextBlock)
            .Text(FText::FromString(TEXT("Asset Tracker")))
        ]

        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SButton)
            .Text(FText::FromString(TEXT("Refresh")))
            .OnClicked_Lambda(
                []()
                {
                    FAssetRegistryModule& AssetRegistryModule =
            		FModuleManager::LoadModuleChecked<FAssetRegistryModule>(
                	TEXT("AssetRegistry"));

        			IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
					TArray<FAssetData> Assets;
        			AssetRegistry.GetAllAssets(Assets, true);

        			UE_LOG(
            			LogTemp,
            			Warning,
            			TEXT("Asset Tracker: Found %d assets"),
            			Assets.Num()
        			);
        			return FReply::Handled();
                })
        ]
		];

	ContentBox->AddSlot()
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Empty")))
		];


	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SBox)
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Top)
			[
				ContentBox
			]
		];
}

void FUE5AssetTrackerModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(UE5AssetTrackerTabName);
}

void FUE5AssetTrackerModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FUE5AssetTrackerCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FUE5AssetTrackerCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUE5AssetTrackerModule, UE5AssetTracker)