// Copyright Ather Labs, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Skill/SipherSkillData.h"
#include "SkillBoostEditor/SipherEdGraph.h"
#include "SipherSkillBoostDataEditor.generated.h"


USTRUCT()
struct FSipherEditedDocumentInfo
{
	GENERATED_BODY()

	/** Edited object */
	UPROPERTY()
	FName DocumentName;

	/** Saved view position */
	UPROPERTY()
	FVector2D SavedViewOffset;

	/** Saved zoom amount */
	UPROPERTY()
	float SavedZoomAmount;

	FSipherEditedDocumentInfo()
		: SavedViewOffset(0.0f, 0.0f)
		, SavedZoomAmount(-1.0f)
	{ }

	FSipherEditedDocumentInfo(const FName& InEditedObject)
		: DocumentName(InEditedObject)
		, SavedViewOffset(0.0f, 0.0f)
		, SavedZoomAmount(-1.0f)
	{ }

	FSipherEditedDocumentInfo(const FName&  InEditedObject, FVector2D& InSavedViewOffset, float InSavedZoomAmount)
		: DocumentName(InEditedObject)
		, SavedViewOffset(InSavedViewOffset)
		, SavedZoomAmount(InSavedZoomAmount)
	{ }

	friend bool operator==(const FSipherEditedDocumentInfo& LHS, const FSipherEditedDocumentInfo& RHS)
	{
		return LHS.DocumentName == RHS.DocumentName && LHS.SavedViewOffset == RHS.SavedViewOffset && LHS.SavedZoomAmount == RHS.SavedZoomAmount;
	}

};

/**
 * 
 */
UCLASS(Config=Editor, PerObjectConfig)
class SIPHERSKILLBOOSTEDITOR_API USipherSkillBoostDataEditor : public UDataAsset
{
public:
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<class USipherSkillData> SkillData;
#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TMap<FName, TObjectPtr<USipherEdGraph>> EdGraphs;
	// Info about the graphs we last edited
	UPROPERTY(Config)
	TArray<FSipherEditedDocumentInfo> LastEditedDocuments;
#endif
};
