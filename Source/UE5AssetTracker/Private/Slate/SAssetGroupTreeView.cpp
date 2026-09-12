#include "Slate/SAssetGroupTreeView.h"
#include "Widgets/Views/SHeaderRow.h"
#include "Widgets/Views/SMultiColumnTableRow.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"

class STreeAssetRow : public SMultiColumnTableRow<FAssetTreeNode::FAssetTreeNodePtr> {
    public:
        SLATE_BEGIN_ARGS(STreeAssetRow) {}
        SLATE_END_ARGS()

        void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTable, FAssetTreeNode::FAssetTreeNodePtr InItem) {
            ItemData = InItem;
            SMultiColumnTableRow<FAssetTreeNode::FAssetTreeNodePtr>::Construct(FSuperRow::FArguments(), InOwnerTable);
        }

        virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override {
            if(ItemData->NodeType == EAssetTreeNodeType::Group) {
                if(ColumnName == "Name") {
                    return SNew(SHorizontalBox)
                    + SHorizontalBox::Slot().AutoWidth()[SNew(SExpanderArrow, SharedThis(this))]
                    + SHorizontalBox::Slot().VAlign_Center(VAlign_Center)
                    [  
                        SNew(STextBlock)
                        .Text(FText::FromString(ItemData->GroupName))
                        .Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
                    ];
                } else if(ColumnName == "Author") {
                    return SNew(STextBlock).Text(FText::FromString(ItemData->Author));
                } else if(ColumnName == "License") {
                    return SNew(STextBlock.Text(FText::FromString(ItemData->License)));
                }
            }
        }
 
    private: 
        FAssetTreeNode::FAssetTreeNodePtr ItemData;
};

    void SAssetGroupTreeView::Construct(const FArguments& InArgs, const TArray<FAssetGroup>& InAssetGroups) {
        SetAssetGroups(InAssetGroups);

        ChildSlot
        [
            SAssignNew(TreeView, STreeView<FAssetTreeNode::FAssetTreeNodePtr>)
            .TreeItemsSource(&RootNodes)
            .OnGenerateRow(this, &SAssetGroupTreeView::OnGenerateRow)
            .OnGetChildren(this, &SAssetGroupTreeView::OnGetChildren)
            .OnMouseDoubleClick(this, &SAssetGroupTreeView::OnMouseDoubleClick)
            .HeaderRow
            (
                SNew(SHeaderRow)
                + SHeaderRow::Column("Name").DefaultLabel(FText::FromString("Name"))
                + SHeaderRow::Column("Author").DefaultLabel(FText::FromString("Author"))
                + SHeaderRow::Column("License").DefaultLabel(FText::FromString("License"))
            )
        ];
    }

    void SAssetGroupTreeView::SetAssetGroups(const TArray<FAssetGroup>& InAssetGroups) {
        RootNodes.Empty();

        for(const FAssetGroup& AssetGroup : InAssetGroups) {
            FAssetTreeNode::FAssetTreeNodePtr GroupNode = MakeShared<FAssetTreeNode>();
            GroupNode->NodeType = EAssetTreeNodeType::Group;
            GroupNode->GroupName = AssetGroup.GroupName;
            GroupNode->Author = AssetGroup.Author;
            GroupNode->License = AssetGroup.License;

            for(const FAssetData& AssetData : AssetGroup.Assets) {
                FAssetTreeNode::FAssetTreeNodePtr AssetNode = MakeShared<FAssetTreeNode>();
                AssetNode->NodeType = EAssetTreeNodeType::Asset;
                AssetNode->AssetData = AssetData;

                GroupNode->Children.Add(AssetNode);
            }

            RootNodes.Add(GroupNode);
        }

        if(TreeView.IsValid()) {
            TreeView->RequestTreeRefresh();
        }
    }

    TSharedRef<ITableRow> SAssetGroupTreeView::OnGenerateRow(FAssetTreeNode::Ptr ItemData, const TSharedRef<STableViewBase>& OwnerTable) {
        return SNew(STreeAssetRow, OwnerTable, ItemData);
    }

    void SAssetGroupTreeView::OnGetChildren(FAssetTreeNode::Ptr ItemData, TArray<FAssetTreeNode::Ptr>& OutChildren) {
        if(ItemData.isValid()) {
            OutChildren = ItemData->Children;
        }
    }

    oid SAssetGroupTreeView::OnTreeItemDoubleClicked(FAssetTreeNode::Ptr ClickedItem)
{
    if (!ClickedItem.IsValid())
    {
        return;
    }

    if (ClickedItem->NodeType == EAssetTreeNodeType::Asset && ClickedItem->AssetData.IsValid())
    {
        FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
        TArray<FAssetData> AssetsToSync;
        AssetsToSync.Add(ClickedItem->AssetData);
        
        ContentBrowserModule.Get().SyncBrowserToObjects(AssetsToSync);
    }
}