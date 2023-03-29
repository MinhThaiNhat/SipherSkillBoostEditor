// Copyright Ather Labs, Inc. All Rights Reserved.


#include "SkillBoostEditor/FSkillBoostAssetEditorToolkit.h"
#include "SkillBoostEditor/SipherAbilityEdGraphNode.h"
#include "SkillBoostEditor/SipherSkillEdGraphSchema.h"
#include "Skill/SipherSkillData.h"
#include "Skill/SipherSkillBoostData.h"
#include "Widgets/Docking/SDockTab.h"
#include "GraphEditor.h"
#include "IStructureDetailsView.h"
#include "Toolkits/IToolkitHost.h"
#include "AssetManagement/SipherAssetManager.h"
#include "DataTableEditorModule.h"
#include "IDataTableEditor.h"
#include "DataTableEditorUtils.h"
#include "Engine/AssetManager.h"
#include "EditorAssetLibrary.h"
#include "AssetToolsModule.h"
#include "Factories/DataAssetFactory.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"
#include "WorkflowOrientedApp/WorkflowUObjectDocuments.h"
#include "BlueprintEditor.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "SkillBoostEditor/SipherEdGraph.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Widgets/Text/SMultiLineEditableText.h"
#include "SourceCodeNavigation.h"
#include "Widgets/Input/SHyperlink.h"

#define LOCTEXT_NAMESPACE "FSkillEditor"
static const FName GraphEditorID = TEXT("Document");

struct FGraphEditorSummoner : public FDocumentTabFactoryForObjects<UEdGraph>
{
public:
	DECLARE_DELEGATE_RetVal_OneParam(TSharedRef<SGraphEditor>, FOnCreateGraphEditorWidget, UEdGraph*);

	FGraphEditorSummoner(TSharedPtr<class FSkillBoostAssetEditorToolkit> InTestEditorPtr, FOnCreateGraphEditorWidget CreateGraphEditorWidgetCallback);
	virtual void OnTabActivated(TSharedPtr<SDockTab> Tab) const override ;
	virtual void OnTabRefreshed(TSharedPtr<SDockTab> Tab) const override ;
	void SaveState(TSharedPtr<SDockTab> Tab, TSharedPtr<FTabPayload> Payload) const override;

protected:
	virtual TAttribute<FText> ConstructTabNameForObject(UEdGraph* DocumentID) const override;
	virtual TSharedRef<SWidget> CreateTabBodyForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const override ;
	virtual const FSlateBrush* GetTabIconForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const override ;
	FOnCreateGraphEditorWidget OnCreateGraphEditorWidget;
};

TAttribute<FText> FGraphEditorSummoner::ConstructTabNameForObject(UEdGraph* DocumentID) const
{
	return FText::FromString(DocumentID->GetName());
}

void FGraphEditorSummoner::OnTabActivated(TSharedPtr<SDockTab> Tab) const
{
	TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(Tab->GetContent());
}
//
//void FGraphEditorSummoner::OnTabRefreshed(TSharedPtr<SDockTab> Tab) const
//{
//	TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(Tab->GetContent());
//	//BlueprintEditorPtr.Pin()->OnGraphEditorBackgrounded(GraphEditor);
//}

void FGraphEditorSummoner::OnTabRefreshed(TSharedPtr<SDockTab> Tab) const
{
	TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(Tab->GetContent());
	GraphEditor->NotifyGraphChanged();
}

void FGraphEditorSummoner::SaveState(TSharedPtr<SDockTab> Tab, TSharedPtr<FTabPayload> Payload) const
{
	TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(Tab->GetContent());

	FVector2D ViewLocation;
	float ZoomAmount;
	GraphEditor->GetViewLocation(ViewLocation, ZoomAmount);

	auto Graph = Payload->IsValid() ? FTabPayload_UObject::CastChecked<USipherEdGraph>(Payload) : nullptr;
	auto GraphAsset = StaticCastSharedPtr<FSkillBoostAssetEditorToolkit>(HostingApp.Pin())->GetGraphAsset();
	if (Graph && GraphAsset)
	{
		GraphAsset->LastEditedDocuments.Add(FSipherEditedDocumentInfo(FName(Graph->GetName()), ViewLocation, ZoomAmount));
	}
}

FGraphEditorSummoner::FGraphEditorSummoner(TSharedPtr<class FSkillBoostAssetEditorToolkit> InBlueprintEditorPtr, FOnCreateGraphEditorWidget CreateGraphEditorWidgetCallback)
	: FDocumentTabFactoryForObjects<UEdGraph>(GraphEditorID, InBlueprintEditorPtr)
, OnCreateGraphEditorWidget(CreateGraphEditorWidgetCallback)
{

}

TSharedRef<SWidget> FGraphEditorSummoner::CreateTabBodyForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const
{
	check(Info.TabInfo.IsValid());
	return OnCreateGraphEditorWidget.Execute(DocumentID);
}

const FSlateBrush* FGraphEditorSummoner::GetTabIconForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const
{
	return FBlueprintEditor::GetGlyphForGraph(DocumentID, false);
}

FSkillBoostAssetEditorToolkit::FSkillBoostAssetEditorToolkit()
{
	DocumentManager = MakeShareable(new FDocumentTracker);
}

FSkillBoostAssetEditorToolkit::~FSkillBoostAssetEditorToolkit()
{
}

void FSkillBoostAssetEditorToolkit::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	DocumentManager->SetTabManager(InTabManager);
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(INVTEXT("Skill Boost Editor"));
	InTabManager->RegisterTabSpawner(FName("AssetPropertyTab"), FOnSpawnTab::CreateRaw(this, &FSkillBoostAssetEditorToolkit::SpawnDetailTab))
		.SetDisplayName(INVTEXT("Details"))
		.SetGroup(WorkspaceMenuCategory.ToSharedRef());
	InTabManager->RegisterTabSpawner(FName("SkillBoostInfoTab"), FOnSpawnTab::CreateRaw(this, &FSkillBoostAssetEditorToolkit::SpawnSkillBoostDetailTab))
		.SetDisplayName(INVTEXT("Skill Boost Details"))
		.SetGroup(WorkspaceMenuCategory.ToSharedRef());
	InTabManager->RegisterTabSpawner(FName("SkillBoostListTab"), FOnSpawnTab::CreateRaw(this, &FSkillBoostAssetEditorToolkit::SpawnSkillBoostTableTab))
		.SetDisplayName(INVTEXT("Boost List"))
		.SetGroup(WorkspaceMenuCategory.ToSharedRef());

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
}

void FSkillBoostAssetEditorToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	InTabManager->UnregisterTabSpawner(FName("AssetPropertyTab"));
	InTabManager->UnregisterTabSpawner(FName("SkillBoostInfoTab"));
	InTabManager->UnregisterTabSpawner(FName("SkillBoostListTab"));
	InTabManager->UnregisterTabSpawner(GraphEditorID);
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
}

void FSkillBoostAssetEditorToolkit::InitializeAssetEditor(const EToolkitMode::Type Mode,
	const TSharedPtr<IToolkitHost>& InitToolkitHost, USipherSkillData* InAssets)
{
	TSharedPtr<FSkillBoostAssetEditorToolkit> ThisPtr(SharedThis(this));
	DocumentManager->Initialize(ThisPtr);
	GraphEditorFactory = MakeShareable(new FGraphEditorSummoner(ThisPtr,
		FGraphEditorSummoner::FOnCreateGraphEditorWidget::CreateSP(this, &FSkillBoostAssetEditorToolkit::CreateGraphEditorWidget)
	));
	DocumentManager->RegisterDocumentFactory(GraphEditorFactory);

	Asset = InAssets;
	LoadGraphAsset();
	GetAllSkillBoost(InAssets);
	for (const auto& Boost : BoostList)
	{
		if (!GraphAsset->EdGraphs.Contains(Boost.Key))
		{
			auto Graph = FBlueprintEditorUtils::CreateNewGraph(GraphAsset, Boost.Key, USipherEdGraph::StaticClass(), USipherSkillEdGraphSchema::StaticClass());
			GraphAsset->EdGraphs.Add(Boost.Key, Cast<USipherEdGraph>(Graph));
		}
		else if (!IsValid(GraphAsset->EdGraphs[Boost.Key]))
		{
			GraphAsset->EdGraphs[Boost.Key] = Cast<USipherEdGraph>(FBlueprintEditorUtils::CreateNewGraph(GraphAsset, Boost.Key, USipherEdGraph::StaticClass(), USipherSkillEdGraphSchema::StaticClass()));
		}
		// Sync data back to graph
		GraphAsset->EdGraphs[Boost.Key]->SyncData(Asset, Boost.Value);
	}

	const TSharedRef<FTabManager::FLayout> StandaloneOurAssetEditor = FTabManager::NewLayout("SkillBoostAssetEditor")
	->AddArea
	(
		FTabManager::NewPrimaryArea()->SetOrientation(EOrientation::Orient_Horizontal)
		->Split(FTabManager::NewStack()->AddTab(GraphEditorID, ETabState::ClosedTab))
		->Split(FTabManager::NewStack()
			->AddTab(FName("AssetPropertyTab"), ETabState::OpenedTab)
			->AddTab(FName("SkillBoostInfoTab"), BoostList.IsEmpty() ? ETabState::ClosedTab : ETabState::OpenedTab)
			->AddTab(FName("SkillBoostListTab"), BoostList.IsEmpty() ? ETabState::ClosedTab : ETabState::OpenedTab)
		)
	);

	InitAssetEditor(Mode, InitToolkitHost, FName("SkillBoostAssetEditor"), StandaloneOurAssetEditor, true, true, InAssets);
	RegenerateMenusAndToolbars();
	TabManager->TryInvokeTab(FName("AssetPropertyTab"));
	GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->NotifyAssetOpened(GraphAsset, this );
	RestoreTab();
}

void FSkillBoostAssetEditorToolkit::OnSelectedNodesChanged(const FName& BoostId, const FGraphPanelSelectionSet& InGrapSelected)
{
	TArray<UObject*> Selection;

	for (UObject* SelectionEntry : InGrapSelected)
	{
		Selection.Add(SelectionEntry);
	}
	if (InGrapSelected.Num() == 1)
	{
		auto V = Cast<USipherAbilityEdGraphNode>(Selection[0]);
		if (!SkillBoostView)
			TabManager->TryInvokeTab(FName("SkillBoostInfoTab"));
		if (SkillBoostView)
		{
			auto Settings = MakeShareable(new FStructOnScope(FSkillBoostInfoEditor::StaticStruct(), (uint8*)&V->GetBoostEditorInfo()));
			SkillBoostView->SetStructureData(Settings);
		}
		TabManager->TryInvokeTab(FName("SkillBoostInfoTab"));
	}
}

void FSkillBoostAssetEditorToolkit::OnNodeDoubleClicked(const FName& BoostId, class UEdGraphNode* InGrapSelected)
{
}

TSharedRef<SDockTab> FSkillBoostAssetEditorToolkit::SpawnDetailTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::Get().LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
	AssetPropertyView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	AssetPropertyView->SetObject(Asset);
	AssetPropertyView->OnFinishedChangingProperties().AddRaw(this, &FSkillBoostAssetEditorToolkit::OnFinishChangeSkillData);
	TSharedRef<SWidget> Message = CreateOptionalDataOnlyMessage();

	auto Box = SNew(SVerticalBox)

		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(FMargin(0,0,0,1))
		[
			Message
		]
	+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		[
			AssetPropertyView.ToSharedRef()
		];

	return SNew(SDockTab)
		[
			Box
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
	auto ListView = SNew(SListView< TSharedPtr<FName> > )
		.ItemHeight(30)
		.ListItemsSource( &Items )
		.OnGenerateRow( this, &FSkillBoostAssetEditorToolkit::GenerateRowWidget)
		.SelectionMode(ESelectionMode::Type::Single)
		.OnMouseButtonDoubleClick(this, &FSkillBoostAssetEditorToolkit::OnDoubleClick)
		//.OnContextMenuOpening( this, &STableViewTesting::GetListContextMenu )
		//.SelectionMode( this, &STableViewTesting::GetSelectionMode )
		.HeaderRow
		(
			SNew(SHeaderRow)
			+ SHeaderRow::Column(FName(TEXT("SkillBoostId")))
			[
				SNew(SBox)
				.VAlign(VAlign_Fill)
				.HAlign(HAlign_Fill)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Skill Boost Id")))
				]
			]
		);
	return SNew(SDockTab)
		[
			ListView
		];
}

void FSkillBoostAssetEditorToolkit::GetAllSkillBoost(USipherSkillData* InAsset)
{
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
		if (BoostData->SkilId != InAsset->SkillName || BoostData->SkillData != InAsset)
			continue;
		Items.Add_GetRef(MakeShareable(new FName(Name)));
		BoostList.Add(Name, BoostData);
	}
}

TSharedRef<ITableRow> FSkillBoostAssetEditorToolkit::GenerateRowWidget(TSharedPtr<FName> InItem, const TSharedRef<class STableViewBase>& OwnerTable)
{
	return SNew(STableRow< TSharedPtr<FName> >, OwnerTable)
		[
			SNew(STextBlock).Text(FText::FromName(*InItem.Get()))
		];
}

void FSkillBoostAssetEditorToolkit::OnDoubleClick(TSharedPtr<FName> InItem)
{
	TSharedRef<FTabPayload_UObject> Payload = FTabPayload_UObject::Make(GraphAsset->EdGraphs[*InItem].Get());
	DocumentManager->OpenDocument(Payload, FDocumentTracker::EOpenDocumentCause::OpenNewDocument);
}

void FSkillBoostAssetEditorToolkit::LoadGraphAsset()
{
	auto FileName = FPaths::GetBaseFilename(Asset->GetPathName()) + "Editor";
	auto EditorAssetPath = FPaths::Combine("/SipherSkillBoostEditor/SkillEditor", FileName + "." + FileName);
	if (UEditorAssetLibrary::DoesAssetExist(EditorAssetPath))
	{
		GraphAsset = Cast<USipherSkillBoostDataEditor>(UEditorAssetLibrary::LoadAsset(EditorAssetPath));
	}
	else
	{
		auto Factory = NewObject<UDataAssetFactory>();
		Factory->DataAssetClass = USipherSkillBoostDataEditor::StaticClass();

		FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");
		UObject* NewAsset = AssetToolsModule.Get().CreateAsset(FileName, FPackageName::GetLongPackagePath(EditorAssetPath), USipherSkillBoostDataEditor::StaticClass(), Factory);
		GraphAsset = Cast<USipherSkillBoostDataEditor>(NewAsset);
	}

	if (!IsValid(GraphAsset))
	{
		GraphAsset = NewObject<USipherSkillBoostDataEditor>(Asset);
	}
}

TSharedRef<SGraphEditor> FSkillBoostAssetEditorToolkit::CreateGraphEditorWidget(UEdGraph* InGraph)
{
	SGraphEditor::FGraphEditorEvents InEvents;
	InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateLambda([this](const FGraphPanelSelectionSet& InGrapSelected)
		{
			OnSelectedNodesChanged("", InGrapSelected);
		});
	InEvents.OnNodeDoubleClicked = FSingleNodeEvent::CreateLambda([this](UEdGraphNode* InNode)
		{
			OnNodeDoubleClicked("", InNode);
		});

	auto Editor = SNew(SGraphEditor).GraphToEdit(InGraph).GraphEvents(InEvents).AssetEditorToolkit(this->AsShared());

	FVector2D ViewOffset = FVector2D::ZeroVector;
	float ZoomAmount = INDEX_NONE;

	TSharedPtr<SDockTab> ActiveTab = DocumentManager->GetActiveTab();
	if(ActiveTab.IsValid())
	{
		// Check if the graph is already opened in the current tab, if it is we want to start at the same position to stop the graph from jumping around oddly
		TSharedPtr<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(ActiveTab->GetContent());

		if(GraphEditor.IsValid() && GraphEditor->GetCurrentGraph() == InGraph)
		{
			GraphEditor->GetViewLocation(ViewOffset, ZoomAmount);
		}
	}

	Editor->SetViewLocation(ViewOffset, ZoomAmount);
	return Editor;
}

void FSkillBoostAssetEditorToolkit::SaveEditedObjectState()
{
	// Clear currently edited documents
	GraphAsset->LastEditedDocuments.Empty();
	//Asset->LastEditedDocuments.Empty();

	// Ask all open documents to save their state, which will update LastEditedDocuments
	DocumentManager->SaveAllState();
}

void FSkillBoostAssetEditorToolkit::OnClose()
{
	// Update the edited object state
	SaveEditedObjectState();
}

void FSkillBoostAssetEditorToolkit::RestoreTab()
{
	if (!GraphAsset)
		return;
	for (auto& Doc : GraphAsset->LastEditedDocuments)
	{
		if (!GraphAsset->EdGraphs.Contains(Doc.DocumentName))
			continue;
		if (USipherEdGraph* Obj = GraphAsset->EdGraphs[Doc.DocumentName])
		{
				TSharedRef<FTabPayload_UObject> Payload = FTabPayload_UObject::Make(Obj);
				TSharedPtr<SDockTab> DocumentTab = DocumentManager->OpenDocument(Payload, FDocumentTracker::OpenNewDocument);
				TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(DocumentTab->GetContent());
				GraphEditor->SetViewLocation(Doc.SavedViewOffset, Doc.SavedZoomAmount);
		}
	}
}


TSharedRef<SWidget> FSkillBoostAssetEditorToolkit::CreateOptionalDataOnlyMessage() const
{
	TSharedRef<SWidget> Message = SNullWidget::NullWidget;
	if (!IsValid(Asset) || Asset->SkillDecription.IsEmpty())
		return Message;
	Message = SNew(SBorder)
		.Padding(FMargin(5))
		.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
		[
			SNew(SMultiLineEditableText)
			.Text(Asset->SkillDecription)
			.AutoWrapText(false)
		
		];

	return Message;
}

void FSkillBoostAssetEditorToolkit::OnFinishChangeSkillData(const FPropertyChangedEvent& EventData)
{
	UClass* Class = USipherSkillData::StaticClass();
	//Class->FindPropertyByName()
}


void FSkillBoostAssetEditorToolkit::PostRegenerateMenusAndToolbars()
{
	
}