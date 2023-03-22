// Copyright Ather Labs, Inc. All Rights Reserved.

#pragma once

#include "Toolkits/AssetEditorToolkit.h"
#include "CoreMinimal.h"

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

public:
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;

	void InitializeAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, class USipherSkillData* InAssets);
	void OnSelectedNodesChanged(const FGraphPanelSelectionSet& InGrapSelected);
	void OnNodeDoubleClicked(class UEdGraphNode* InGrapSelected);
	void GetAllSkillBoost();

private:

	TSharedRef<SDockTab> SpawnDetailTab(const FSpawnTabArgs& SpawnTabArgs);
	TSharedRef<SDockTab> SpawnSkillBoostDetailTab(const FSpawnTabArgs& SpawnTabArgs);
	TSharedRef<SDockTab> SpawnSkillBoostTableTab(const FSpawnTabArgs& SpawnTabArgs);
	TObjectPtr<UEdGraph> EdGraph = {};
	USipherSkillData* Asset;
	TSharedPtr<SGraphEditor> GraphEditor;
	TSharedPtr<class IDetailsView> AssetPropertyView;
	TSharedPtr<class IStructureDetailsView> SkillBoostView;
	inline static const FName GraphEditorTabName {"OurAssetsGraphEditor"};
	inline static const FName PropertyEditorTabName {"OurAssetsPropertyEditor"};
	/** Temporary data table to use to display import options */
	UPROPERTY()
	TObjectPtr<UDataTable> DataTableImportOptions;
	TArray<TSharedPtr<FName>> Items;
private:
	void CreateAbilityNode(UEdGraph* ParentGraph, const FVector2D NodeLocation);
};
