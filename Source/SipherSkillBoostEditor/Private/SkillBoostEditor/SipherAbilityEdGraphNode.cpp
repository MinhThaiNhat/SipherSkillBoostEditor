// Copyright Ather Labs, Inc. All Rights Reserved.

#include "SkillBoostEditor/SipherAbilityEdGraphNode.h"
#include "Skill/SummonTemplate/SipherSummonAbility.h"

void USipherAbilityEdGraphNode::SetAbility(const FName& InAbilityAtlasName, const FSipherSubSkillInfo& InInfo)
{
	AbilityAtlasName = InAbilityAtlasName;
	Info = InInfo;
	Settings = MakeShareable(new FStructOnScope(FSipherSubSkillInfo::StaticStruct(), (uint8*)&Info));

	//Settings = MakeShared<TStructOnScope<FSipherSubSkillInfo>>(reinterpret_cast<uint8*>(&Info));
}

void USipherAbilityEdGraphNode::AllocateDefaultPins()
{
	if (!Info.SkillAbilityClass)
		return;
	CreatePin(EGPD_Input, PinCategory, TEXT("Exec"));

	CreatePin(EGPD_Output, PinCategory, TEXT("Pre Activate"));
	CreatePin(EGPD_Output, PinCategory, TEXT("Activated"));
	CreatePin(EGPD_Output, PinCategory, TEXT("End"));
	if (Info.SkillAbilityClass->GetDefaultObject()->IsA(USipherSummonBaseAbility::StaticClass()))
	{
		CreatePin(EGPD_Output, PinCategory, TEXT("Summoned"));
		CreatePin(EGPD_Output, PinCategory, TEXT("SummonRecall"));
		CreatePin(EGPD_Output, PinCategory, TEXT("AllSummonRecall"));
	}
}

FText USipherAbilityEdGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromName(AbilityAtlasName);
}

FLinearColor USipherAbilityEdGraphNode::GetNodeTitleColor() const
{
	if (AbilityAtlasName == "Base")
		return FLinearColor::Red;
	else
		return FLinearColor::Yellow;
}

FText USipherAbilityEdGraphNode::GetTooltipText() const
{
	return NSLOCTEXT("EditorExtenstion", "Our Graph Node Tooltip", "Our Graph Node Tooltip");
}