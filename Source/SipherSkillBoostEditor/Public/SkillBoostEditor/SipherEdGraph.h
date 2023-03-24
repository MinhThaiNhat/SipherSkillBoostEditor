// Copyright Ather Labs, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "Skill/SipherSkillBoostData.h"
#include "SipherEdGraph.generated.h"

/**
 * 
 */
UCLASS()
class SIPHERSKILLBOOSTEDITOR_API USipherEdGraph : public UEdGraph
{
	GENERATED_BODY()
public:
	void SyncData(class USipherSkillData* SkillData, FSipherSkillBoostData* BoostData);
};
