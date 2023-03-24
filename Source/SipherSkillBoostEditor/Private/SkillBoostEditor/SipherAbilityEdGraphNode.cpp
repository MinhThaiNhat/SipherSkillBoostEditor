// Copyright Ather Labs, Inc. All Rights Reserved.

#include "SkillBoostEditor/SipherAbilityEdGraphNode.h"
#include "Skill/SummonTemplate/SipherSummonAbility.h"
#include "Skill/SipherSkillData.h"
#include "Skill/SipherSkillBoostData.h"

void USipherAbilityEdGraphNode::SetAbility(const FName& InAbilityAtlasName, const FSipherSubSkillInfo& InInfo)
{
	AbilityAtlasName = InAbilityAtlasName;
	Info = InInfo;
	Settings = MakeShareable(new FStructOnScope(FSipherSubSkillInfo::StaticStruct(), (uint8*)&Info));

	//Settings = MakeShared<TStructOnScope<FSipherSubSkillInfo>>(reinterpret_cast<uint8*>(&Info));
}

void USipherAbilityEdGraphNode::SetBoost(FSipherSkillBoostData* BoostData)
{
	if (BoostData)
		BoostInfo = *BoostData;
}
template<class EnumType>
static FORCEINLINE EnumType GetEnumValueFromString(const FString& EnumName, const FName& String)
{
	UEnum* Enum = StaticEnum<EnumType>();
	if(!Enum)
	{ 
		return EnumType(0);
	}		
	return (EnumType)Enum->GetValueByNameString(String.ToString());
}

UEdGraphPin* USipherAbilityEdGraphNode::FindPin(ESkillPhase Phase)
{
	if (SkillPhaseToPin.Contains(Phase))
		return SkillPhaseToPin[Phase];

	for (auto Pin : Pins)
	{
		auto OutPhase = GetEnumValueFromString<ESkillPhase>("ESkillPhase", Pin->PinType.PinSubCategory);
		if (OutPhase == Phase)
		{
			return Pin;
		}
	}
	return nullptr;
}

void USipherAbilityEdGraphNode::AllocateDefaultPins()
{
	if (!Info.SkillAbilityClass)
		return;
	SkillPhaseToPin.Add(ESkillPhase::ESP_None, CreatePin(EGPD_Input, PinCategory, "ESkillPhase::ESP_None", TEXT("Exec")));

	SkillPhaseToPin.Add(ESkillPhase::ESP_PreActivate, CreatePin(EGPD_Output, PinCategory, "ESkillPhase::ESP_PreActivate", TEXT("Pre Activate")));
	SkillPhaseToPin.Add(ESkillPhase::ESP_Activate, CreatePin(EGPD_Output, PinCategory, "ESkillPhase::ESP_Activate", TEXT("Activated")));
	SkillPhaseToPin.Add(ESkillPhase::ESP_End, CreatePin(EGPD_Output, PinCategory, "ESkillPhase::ESP_End", TEXT("End")));
	if (Info.SkillAbilityClass->GetDefaultObject()->IsA(USipherSummonBaseAbility::StaticClass()))
	{
		SkillPhaseToPin.Add(ESkillPhase::ESP_Summon, CreatePin(EGPD_Output, PinCategory, "ESkillPhase::ESP_Summon", TEXT("Summoned")));
		SkillPhaseToPin.Add(ESkillPhase::ESP_SummonFinish, CreatePin(EGPD_Output, PinCategory, "ESkillPhase::ESP_SummonFinish", TEXT("SummonRecall")));
		SkillPhaseToPin.Add(ESkillPhase::ESP_SummonFinishAll, CreatePin(EGPD_Output, PinCategory, "ESkillPhase::ESP_SummonFinishAll", TEXT("AllSummonRecall")));
	}
}

FText USipherAbilityEdGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromName(AbilityAtlasName);
}

FLinearColor USipherAbilityEdGraphNode::GetNodeTitleColor() const
{

	for (auto Pin : Pins)
	{
		if (Pin->HasAnyConnections())
		{
			if (AbilityAtlasName == "Base")
				return FLinearColor::Red;
			else
				return FLinearColor::Yellow;
		}
	}

	if (AbilityAtlasName == "Base")
	{
		auto Color = FLinearColor::Red;
		if (BoostInfo.SkillParamModifierInfo.IsEmpty() && !IsValid(*BoostInfo.TargetingType))
		{
			Color.A = 0.2;
		}
		return Color;
	}
	else
	{
		auto Color = FLinearColor::Yellow;
		if (!BoostInfo.AbilityAndConditionToActivate.Contains(GetAbilityName()))
		{
			Color.A = 0.2;
		}
		return Color;
	}
}

FLinearColor USipherAbilityEdGraphNode::GetNodeBodyTintColor() const
{
	for (auto Pin : Pins)
	{
		if (Pin->HasAnyConnections())
			return FLinearColor::White;
	}
	auto Color = FLinearColor::White;
	if (AbilityAtlasName == "Base")
	{
		if (BoostInfo.SkillParamModifierInfo.IsEmpty() && !IsValid(*BoostInfo.TargetingType))
		{
			Color.A = 0.2;
		}
	}
	else
	{
		if (!BoostInfo.AbilityAndConditionToActivate.Contains(GetAbilityName()))
		{
			Color.A = 0.2;
		}
	}
	return Color;
}

FText USipherAbilityEdGraphNode::GetTooltipText() const
{
	return NSLOCTEXT("EditorExtenstion", "Our Graph Node Tooltip", "Our Graph Node Tooltip");
}