#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/STreeView.h"
#include "AssetRegistry/AssetData.h"
#include "AssetGroup.h"

enum class EAssetTreeNodeType {
    Group,
    Asset
};

struct FAssetTreeNode {
    FString GroupName;
    FString Author;
    FString License;

    FAssetData AssetData;

    TArray<TSharedPtr<FAssetTreeNode>> Children;

    EAssetTreeNodeType NodeType = EAssetTreeNodeType::Group;
    typedef TSharedPtr<FAssetTreeNode> FAssetTreeNodePtr;
};

class UE5ASSETTRACKER_API SAssetGroupTreeView : public SCompoundWidget {
    public:
        SLATE_BEGIN_ARGS(SAssetGroupTreeView) {}
        SLATE_END_ARGS()

        void Construct(const FArguments& InArgs, const TArray<FAssetGroup>& InAssetGroups);

        void SetAssetGroups(const TArray<FAssetGroup>& InAssetGroups);

    private:
        TSharedRef<ITableRow> OnGenerateRow(FAssetTreeNode::FAssetTreeNodePtr Item, const TSharedRef<STableViewBase>& OwnerTable);
        void OnGetChildren(FAssetTreeNode::FAssetTreeNodePtr Item, TArray<FAssetTreeNode::FAssetTreeNodePtr>& OutChildren);
        void OnTreeItemDoubleClicked(FAssetTreeNode::FAssetTreeNodePtr clickedItem);

    private:
        TArray<FAssetTreeNode::FAssetTreeNodePtr> RootNodes;
        TSharedPtr<STreeView<FAssetTreeNode::FAssetTreeNodePtr>> TreeView;
};
