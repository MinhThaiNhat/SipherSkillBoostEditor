// Copyright Ather Labs, Inc. All Rights Reserved.


#include "SkillBoostEditor/FSkillBoostAssetEditorToolkit.h"
#include "SkillBoostEditor/SipherAbilityEdGraphNode.h"
#include "SkillBoostEditor/SipherSkillEdGraphSchema.h"
#include "Skill/SipherSkillData.h"
#include "Widgets/Docking/SDockTab.h"
#include "GraphEditor.h"
#include "IStructureDetailsView.h"
#include "Toolkits/IToolkitHost.h"
#include "AssetManagement/SipherAssetManager.h"
#include "Skill/SipherSkillBoostData.h"
#include "DataTableEditorModule.h"
#include "IDataTableEditor.h"
#include "DataTableEditorUtils.h"
#include "Engine/AssetManager.h"

#define LOCTEXT_NAMESPACE "FSkillEditor"

FSkillBoostAssetEditorToolkit::FSkillBoostAssetEditorToolkit()
{
}

FSkillBoostAssetEditorToolkit::~FSkillBoostAssetEditorToolkit()
{
}

void FSkillBoostAssetEditorToolkit::CreateAbilityNode(UEdGraph* ParentGraph, const FVector2D NodeLocation)
{
	check(ParentGraph != nullptr);
	float SpanDegree = 360.0f / (Asset->SkillAbility.SkillAbilityClasses.Num() - 1);
	float Distance = 200;
	float Angle = 0;
	for (auto& Ability : Asset->SkillAbility.SkillAbilityClasses)
	{
		USipherAbilityEdGraphNode* ResultGraphNode = NewObject<USipherAbilityEdGraphNode>(ParentGraph);
		ParentGraph->Modify();
		ResultGraphNode->SetFlags(RF_Transactional);
		ResultGraphNode->SetAbility(Ability.Key, Ability.Value);
		ResultGraphNode->Rename(nullptr, ParentGraph, REN_NonTransactional);
		ResultGraphNode->CreateNewGuid();
		if (Ability.Key != "Base")
		{
			ResultGraphNode->NodePosX = FMath::Cos(Angle) * Distance + NodeLocation.X;
			ResultGraphNode->NodePosY = FMath::Sin(Angle) * Distance + NodeLocation.Y;
		}
		else
		{
			ResultGraphNode->NodePosX = NodeLocation.X;
			ResultGraphNode->NodePosY = NodeLocation.Y;
		}

		ResultGraphNode->AllocateDefaultPins();
		ParentGraph->AddNode(ResultGraphNode);
		Angle += SpanDegree;
	}
}

void FSkillBoostAssetEditorToolkit::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(INVTEXT("Skill Boost Editor"));
	InTabManager->RegisterTabSpawner(FName("AssetPropertyTab"), FOnSpawnTab::CreateRaw(this, &FSkillBoostAssetEditorToolkit::SpawnDetailTab))
		.SetDisplayName(INVTEXT("Details"))
		.SetGroup(WorkspaceMenuCategory.ToSharedRef());
	InTabManager->RegisterTabSpawner(FName("SkillBoostInfoTab"), FOnSpawnTab::CreateRaw(this, &FSkillBoostAssetEditorToolkit::SpawnSkillBoostDetailTab))
		.SetDisplayName(INVTEXT("Skill Boost Details"))
		.SetGroup(WorkspaceMenuCategory.ToSharedRef());
	InTabManager->RegisterTabSpawner(FName("SkillBoostListTab"), FOnSpawnTab::CreateRaw(this, &FSkillBoostAssetEditorToolkit::SpawnSkillBoostTableTab))
		.SetDisplayName(INVTEXT("Skill Boost Table"))
		.SetGroup(WorkspaceMenuCategory.ToSharedRef());

	SGraphEditor::FGraphEditorEvents InEvents;
	InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FSkillBoostAssetEditorToolkit::OnSelectedNodesChanged);
	InEvents.OnNodeDoubleClicked = FSingleNodeEvent::CreateSP(this, &FSkillBoostAssetEditorToolkit::OnNodeDoubleClicked);
	GraphEditor = SNew(SGraphEditor).GraphToEdit(EdGraph)
			.GraphEvents(InEvents);

	InTabManager->RegisterTabSpawner(FName("SkillBoostEditorTab"), FOnSpawnTab::CreateLambda([&](const FSpawnTabArgs& SpawnTabArgs)
		{
			return SNew(SDockTab)
				[
					GraphEditor.ToSharedRef()
				];
		}));
}

void FSkillBoostAssetEditorToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	InTabManager->UnregisterTabSpawner(FName("AssetPropertyTab"));
	InTabManager->UnregisterTabSpawner(FName("SkillBoostEditorTab"));
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
}

void FSkillBoostAssetEditorToolkit::InitializeAssetEditor(const EToolkitMode::Type Mode,
	const TSharedPtr<IToolkitHost>& InitToolkitHost, USipherSkillData* InAssets)
{
	Asset = InAssets;
	EdGraph = NewObject<UEdGraph>();
	EdGraph->Schema = USipherSkillEdGraphSchema::StaticClass();
	EdGraph->AddToRoot();

	CreateAbilityNode(EdGraph, {0, 0});

	const TSharedRef<FTabManager::FLayout> StandaloneOurAssetEditor = FTabManager::NewLayout("SkillBoostAssetEditor")
	->AddArea
	(
		FTabManager::NewPrimaryArea()->SetOrientation(EOrientation::Orient_Horizontal)
		->Split(FTabManager::NewStack()
			->AddTab(FName("AssetPropertyTab"), ETabState::OpenedTab)
			->AddTab(FName("SkillBoostInfoTab"), ETabState::OpenedTab)
			->AddTab(FName("SkillBoostListTab"), ETabState::OpenedTab)
		)
		->Split(FTabManager::NewStack()->AddTab(FName("SkillBoostEditorTab"), ETabState::OpenedTab))
	);

	InitAssetEditor(Mode, InitToolkitHost, FName("SkillBoostAssetEditor"), StandaloneOurAssetEditor, true, true, InAssets);
	RegenerateMenusAndToolbars();
	GetAllSkillBoost();
}

void FSkillBoostAssetEditorToolkit::OnSelectedNodesChanged(const FGraphPanelSelectionSet& InGrapSelected)
{
	TArray<UObject*> Selection;

	for (UObject* SelectionEntry : InGrapSelected)
	{
		Selection.Add(SelectionEntry);
	}
	if (Selection.Num() == 0) 
	{
		GetAssociatedTabManager()->TryInvokeTab(FName("AssetPropertyTab"));
	}
	else if (InGrapSelected.Num() == 1)
	{
		auto V = Cast<USipherAbilityEdGraphNode>(Selection[0]);
		if (!SkillBoostView)
			GetAssociatedTabManager()->TryInvokeTab(FName("SkillBoostInfoTab"));
		if (SkillBoostView)
			SkillBoostView->SetStructureData(V->Settings);
		GetAssociatedTabManager()->TryInvokeTab(FName("SkillBoostInfoTab"));
	}
}

void FSkillBoostAssetEditorToolkit::OnNodeDoubleClicked(class UEdGraphNode* InGrapSelected)
{
}

TSharedRef<SDockTab> FSkillBoostAssetEditorToolkit::SpawnDetailTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::Get().LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
	AssetPropertyView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	AssetPropertyView->SetObject(GetEditingObject());
	return SNew(SDockTab)
		[
			AssetPropertyView.ToSharedRef()
		];
}

TSharedRef<SDockTab> FSkillBoostAssetEditorToolkit::SpawnSkillBoostDetailTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::Get().LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;

	FStructureDetailsViewArgs StructureViewArgs;

	SkillBoostView = PropertyEditorModule.CreateStructureDetailView(DetailsViewArgs, StructureViewArgs, TSharedPtr<FStructOnScope>());
	return SNew(SDockTab)
		[
			SkillBoostView->GetWidget().ToSharedRef()
		];
}

TSharedRef<SDockTab> FSkillBoostAssetEditorToolkit::SpawnSkillBoostTableTab(const FSpawnTabArgs& SpawnTabArgs)
{
	auto& DataTableEditorModule = FModuleManager::LoadModuleChecked<FDataTableEditorModule>("DataTableEditor");
	auto ListView = SNew(SListView< TSharedPtr<FName> > )
		.ItemHeight(24)
		.ListItemsSource( &Items )
		//.OnGenerateRow( this, &STableViewTesting::OnGenerateWidgetForList )
		//.OnContextMenuOpening( this, &STableViewTesting::GetListContextMenu )
		//.SelectionMode( this, &STableViewTesting::GetSelectionMode )
		.HeaderRow
		(
			SNew(SHeaderRow)
			+ SHeaderRow::Column(FName(TEXT("SkillBoostId")))
			[
				SNew(SBorder)
				.Padding(5)
			.Content()
			[
				SNew(STextBlock)
				.Text(LOCTEXT("FSkillEditor", "SkillBoostId2"))
			]
			]
		);
	return SNew(SDockTab)
		[
			ListView
		];
}

void FSkillBoostAssetEditorToolkit::GetAllSkillBoost()
{
	DataTableImportOptions = NewObject<UDataTable>(GetEditingObject());
	TSoftObjectPtr<UDataTable> Path(FSoftObjectPath(TEXT("/SipherGameModule/Data/DT_SkillBoost.DT_SkillBoost")));
	auto DataTable = Path.LoadSynchronous();
	if (!IsValid(DataTable))
		return;
	auto RowNames = DataTable->GetRowNames();
	for (auto Name : RowNames)
	{
		auto BoostData = DataTable->FindRow<FSipherSkillBoostData>(Name, "");
		if (!BoostData)
			continue;
		Items.Add_GetRef(MakeShareable(new FName(Name)));
	}
}