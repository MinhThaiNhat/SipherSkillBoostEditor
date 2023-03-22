// Copyright Ather Labs, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "Skill/SipherSkillData.h"
#include "SipherAbilityEdGraphNode.generated.h"

/**
 * 
 */
UCLASS()
class SIPHERSKILLBOOSTEDITOR_API USipherAbilityEdGraphNode : public UEdGraphNode
{
	GENERATED_BODY()
public:
	inline static FName PinCategory {"PinAbilityEventCategory"};
	inline static FName PinSubCategory {"PinAbilityEventSubCategory"};
	void SetAbility(const FName& AbilityAtlasName, const struct FSipherSubSkillInfo& Info);
	/** Begin UEdGraphNode Interface */
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetTooltipText() const override;

	TSharedPtr<FStructOnScope> Settings;
protected:

	FName AbilityAtlasName;
	struct FSipherSubSkillInfo Info;
};
