// Copyright Ather Labs, Inc. All Rights Reserved.

#include "SkillBoostEditor/SipherAbilityEdGraphNode.h"
#include "Skill/SummonTemplate/SipherSummonAbility.h"
#include "Skill/SipherSkillData.h"
#include "Skill/SipherSkillBoostData.h"

void USipherAbilityEdGraphNode::SetAbility(const FName& InAbilityAtlasName, const FSipherSubSkillInfo& InInfo)
{
	AbilityAtlasName = InAbilityAtlasName;
	SkillAbilityClass = InInfo.SkillAbilityClass;
}

void USipherAbilityEdGraphNode::SetBoost(FSipherSkillBoostData* BoostData)
{
	if (!BoostData)
		return;
	if (AbilityAtlasName == "Base")
	{
		TargetingType = BoostData->TargetingType;
		SkillParamModifierInfo = BoostData->SkillParamModifierInfo;
	}
	else if (BoostData->AbilityAndConditionToActivate.Contains(AbilityAtlasName))
	{
		TargetingType = BoostData->AbilityAndConditionToActivate[AbilityAtlasName].TargetingType;
		SkillParamModifierInfo = BoostData->AbilityAndConditionToActivate[AbilityAtlasName].SkillParamModifierInfo;
	}
}

void USipherAbilityEdGraphNode::Sync()
{
	bool bHasAnyConnection = false;
	for (auto Pin : Pins)
	{
		if (Pin->HasAnyConnections())
		{
			bHasAnyConnection = true;
			break;
		}
	}

	if (!bHasAnyConnection && SkillParamModifierInfo.IsEmpty())
		MakeAutomaticallyPlacedGhostNode();
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
			SkillPhaseToPin.Add(OutPhase, Pin);
			return Pin;
		}
	}
	return nullptr;
}

FSipherSkillBoostData USipherAbilityEdGraphNode::BuildBoostInfo()
{
	return FSipherSkillBoostData();
}

void USipherAbilityEdGraphNode::AllocateDefaultPins()
{
	SkillPhaseToPin.Add(ESkillPhase::ESP_None, CreatePin(EGPD_Input, PinCategory, "ESkillPhase::ESP_None", TEXT("Exec")));

	SkillPhaseToPin.Add(ESkillPhase::ESP_PreActivate, CreatePin(EGPD_Output, PinCategory, "ESkillPhase::ESP_PreActivate", TEXT("Pre Activate")));
	SkillPhaseToPin.Add(ESkillPhase::ESP_Activate, CreatePin(EGPD_Output, PinCategory, "ESkillPhase::ESP_Activate", TEXT("Activated")));
	SkillPhaseToPin.Add(ESkillPhase::ESP_End, CreatePin(EGPD_Output, PinCategory, "ESkillPhase::ESP_End", TEXT("End")));
	if (SkillAbilityClass && SkillAbilityClass->GetDefaultObject()->IsA(USipherSummonBaseAbility::StaticClass()))
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
		return FLinearColor::Red;
	}
	else
	{
		return FLinearColor::Yellow;
	}
}

FLinearColor USipherAbilityEdGraphNode::GetNodeBodyTintColor() const
{
	return Super::GetNodeBodyTintColor();
}

FText USipherAbilityEdGraphNode::GetTooltipText() const
{
	return FText::FromString(AbilityAtlasName.ToString());
}