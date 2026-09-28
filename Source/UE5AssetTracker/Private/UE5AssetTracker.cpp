// Copyright Epic Games, Inc. All Rights Reserved.

#include "../Public/UE5AssetTracker.h"
#include "UE5AssetTrackerStyle.h"
#include "UE5AssetTrackerCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "ToolMenus.h"
#include "Slate/SAssetGroupTreeView.h"
#include "AssetGroup.h"

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

	AssetTreeView.Reset();

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
            .OnClicked_Raw(this, &FUE5AssetTrackerModule::OnRefreshClicked)
        ]
		];

	ContentBox->AddSlot()
		.FillHeight(1.0f)
		[
			SAssignNew(AssetTreeView, SAssetGroupTreeView, BuildAssetGroups())
		];

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			ContentBox
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

TArray<FAssetGroup> FUE5AssetTrackerModule::BuildAssetGroups() const {
    IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();

    FARFilter Filter;
    Filter.PackagePaths.Add("/Game");
    Filter.bRecursivePaths = true;

    TArray<FAssetData> Assets;
    AssetRegistry.GetAssets(Filter, Assets);

    // Temporary grouping until the real data source exists: one group per top-level /Game folder
    TMap<FString, FAssetGroup> GroupsByFolder;
    for (const FAssetData& Asset : Assets) {
        TArray<FString> Parts;
        Asset.PackagePath.ToString().ParseIntoArray(Parts, TEXT("/"));
        const FString Folder = Parts.Num() > 1 ? Parts[1] : TEXT("(root)");

        FAssetGroup& Group = GroupsByFolder.FindOrAdd(Folder);
        Group.GroupName = Folder;
        Group.Assets.Add(Asset);
    }

    TArray<FAssetGroup> Result;
    GroupsByFolder.GenerateValueArray(Result);
    return Result;
}

FReply FUE5AssetTrackerModule::OnRefreshClicked() {
    if (AssetTreeView.IsValid()) {
        AssetTreeView->SetAssetGroups(BuildAssetGroups());
    }
    return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUE5AssetTrackerModule, UE5AssetTracker)