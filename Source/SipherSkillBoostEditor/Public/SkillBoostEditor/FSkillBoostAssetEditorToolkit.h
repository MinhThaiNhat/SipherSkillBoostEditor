// Copyright Ather Labs, Inc. All Rights Reserved.

#pragma once

#include "Toolkits/AssetEditorToolkit.h"
#include "SkillBoostEditor/SipherSkillBoostDataEditor.h"
#include "CoreMinimal.h"

class FSkillEditorCommands : public TCommands<FSkillEditorCommands>
{
public:
	/** Constructor */
	FSkillEditorCommands() 
		: TCommands<FSkillEditorCommands>("SkillEditorCommands", NSLOCTEXT("Contexts", "SkillEditorCommands", "Skill Editor"), NAME_None, FEditorStyle::GetStyleSetName())
	{
	}

	/** Compile the blueprint */
	TSharedPtr<FUICommandInfo> SaveBoost;
	TSharedPtr<FUICommandInfo> NewSkillBoost;

	/** Initialize commands */
	virtual void RegisterCommands() override;
};

/**
 * 
 */
class SIPHERSKILLBOOSTEDITOR_API FSkillBoostAssetEditorToolkit : public FAssetEditorToolkit
{
public:
	FSkillBoostAssetEditorToolkit();
	~FSkillBoostAssetEditorToolkit();
public:
	virtual FName GetToolkitFName() const override { return FName("OurAssetsEditorToolkit"); }
	virtual FText GetBaseToolkitName() const override { return NSLOCTEXT("EditorExtension", "Out Asset Toolkit Name", "SkillBoostToolKit"); }
	virtual FString GetWorldCentricTabPrefix() const override { return NSLOCTEXT("EditorExtension", "Out Asset Toolkit Tab Prefix", "SkillBoost").ToString(); }
	virtual FLinearColor GetWorldCentricTabColorScale() const override { return FLinearColor::Green; }
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;

public:
	USipherSkillBoostDataEditor* GetGraphAsset() { return GraphAsset; };
	USipherSkillData* GetAsset() { return Asset; };

	void InitializeAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, class USipherSkillData* InAssets);
	void OnSelectedNodesChanged(const FName& BoostId, const FGraphPanelSelectionSet& InGrapSelected);
	void OnNodeDoubleClicked(const FName& BoostId, class UEdGraphNode* InGrapSelected);
	void GetAllSkillBoost(USipherSkillData* InAsset);
	TSharedRef<ITableRow> GenerateRowWidget(TSharedPtr<FName> InItem, const TSharedRef<class STableViewBase>& OwnerTable);
	void OnDoubleClick(TSharedPtr<FName> InItem);
	void LoadGraphAsset();
	TSharedRef<SGraphEditor> CreateGraphEditorWidget(UEdGraph* InGraph);
	TSharedRef<SWidget> CreateOptionalDataOnlyMessage();
	void PostRegenerateMenusAndToolbars() override;

private:
	void CommandNewBoost();
	void CommandSaveBoost();
	void OnFinishChangeSkillData(const FPropertyChangedEvent& Event);
	void OnFinishChangeBoostData(const FPropertyChangedEvent& EventData);
	void OnClose() override;
	void SaveEditedObjectState();
	void RestoreTab();
	void OnSkillDescriptionCommited(const FText& InFilterText, ETextCommit::Type InCommitType);

	TSharedRef<SDockTab> SpawnDetailTab(const FSpawnTabArgs& SpawnTabArgs);
	TSharedRef<SDockTab> SpawnSkillBoostDetailTab(const FSpawnTabArgs& SpawnTabArgs);
	TSharedRef<SDockTab> SpawnSkillBoostTableTab(const FSpawnTabArgs& SpawnTabArgs);

	UPROPERTY()
	USipherSkillBoostDataEditor* GraphAsset;
	USipherSkillData* Asset;
	TMap<FName, TSharedPtr<SGraphEditor>> GraphEditors;
	TSharedPtr<class IDetailsView> AssetPropertyView;
	TSharedPtr<class IDetailsView> SkillBoostView;
	/** Temporary data table to use to display import options */
	UPROPERTY()
	TObjectPtr<UDataTable> DataTableImportOptions;
	TArray<TSharedPtr<FName>> Items;
	TMap<FName, struct FSipherSkillBoostData*> BoostList;
	TSharedPtr<class FDocumentTracker> DocumentManager;
	TSharedPtr<class FDocumentTabFactory> GraphEditorFactory;

};
