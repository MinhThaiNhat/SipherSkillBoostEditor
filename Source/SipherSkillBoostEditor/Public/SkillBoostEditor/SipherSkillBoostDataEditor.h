// Copyright Ather Labs, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Skill/SipherSkillData.h"
#include "SkillBoostEditor/SipherEdGraph.h"
#include "SipherSkillBoostDataEditor.generated.h"

/**
 * 
 */
UCLASS()
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
	UPROPERTY()
	TArray<FEditedDocumentInfo> LastEditedDocuments;
#endif
};
