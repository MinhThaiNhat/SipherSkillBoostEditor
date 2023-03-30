// Copyright Ather Labs, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "Skill/SipherSkillData.h"
#include "Skill/SipherSkillBoostData.h"
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
	void SetBoost(struct FSipherSkillBoostData* BoostData);
	void Sync();

	/** Begin UEdGraphNode Interface */
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FLinearColor GetNodeBodyTintColor() const;
	virtual FText GetTooltipText() const override;

	/*TSharedPtr<SGraphNode> CreateVisualWidget() override
	{
		return SNew(SGraphNodeCreateWidget, this);
	}*/

	UEdGraphPin* FindPin(ESkillPhase Phase);
	const FName& GetAbilityName() const { return AbilityAtlasName; };
	FSipherSkillBoostData BuildBoostInfo();
	TSharedPtr<FStructOnScope> Settings;
public:
	UPROPERTY(EditAnywhere)
	FName AbilityAtlasName;

	UPROPERTY(EditAnywhere)
	TArray<FSipherSkillParamModifierInfo> SkillParamModifierInfo;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class USipherAbilityTargetType> TargetingType;

	TMap<ESkillPhase, UEdGraphPin*> SkillPhaseToPin;
	TSubclassOf<USipherSkillAbility> SkillAbilityClass = nullptr;
};
