// Copyright Ather Labs, Inc. All Rights Reserved.


#include "SkillBoostEditor/SipherEdGraph.h"
#include "SkillBoostEditor/SipherAbilityEdGraphNode.h"
#include "Skill/SipherSkillData.h"
#include "Skill/SipherSkillBoostData.h"

void USipherEdGraph::SyncData(USipherSkillData* SkillData, FSipherSkillBoostData* BoostData)
{
	if (!IsValid(SkillData) || BoostData == nullptr)
		return;
	TArray<USipherAbilityEdGraphNode*> ListNodes;

	// Map ability to its node
	TMap<FName, USipherAbilityEdGraphNode*> AbilityToNode;
	GetNodesOfClass(ListNodes);
	TSet<FName> AbilitiesToAdd;
	auto& AbilityInfos = SkillData->SkillAbility.SkillAbilityClasses;
	AbilityInfos.GetKeys(AbilitiesToAdd);
	USipherAbilityEdGraphNode* BaseNode = nullptr;
	for (auto Node : ListNodes)
	{
		if (!AbilityInfos.Contains(Node->GetAbilityName()))
		{
			RemoveNode(Node);
			continue;
		}
		Node->SetAbility(Node->GetAbilityName(), AbilityInfos[Node->GetAbilityName()]);
		Node->SetBoost(BoostData);
		AbilitiesToAdd.Remove(Node->GetAbilityName());
		if (Node->GetAbilityName() == "Base")
			BaseNode = Node;
		//Node->AllocateDefaultPins();
		Node->BreakAllNodeLinks();
		AbilityToNode.Add(Node->GetAbilityName(), Node);
	}

	FVector2D NodeLocation = FVector2D::ZeroVector;
	if (IsValid(BaseNode))
	{
		NodeLocation.X = BaseNode->NodePosX;
		NodeLocation.Y = BaseNode->NodePosY;
	}
	float SpanDegree = 360.0f / (AbilityInfos.Num() - 1);
	float Distance = 300;
	float Angle = 0;
	for (auto& Ability : AbilitiesToAdd)
	{
		USipherAbilityEdGraphNode* ResultGraphNode = NewObject<USipherAbilityEdGraphNode>(this, Ability);
		this->Modify();
		ResultGraphNode->SetFlags(RF_Transactional);
		ResultGraphNode->SetAbility(Ability, AbilityInfos[Ability]);
		ResultGraphNode->SetBoost(BoostData);
		ResultGraphNode->CreateNewGuid();
		if (Ability != "Base")
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
		AddNode(ResultGraphNode);
		Angle += SpanDegree;
		AbilityToNode.Add(ResultGraphNode->GetAbilityName(), ResultGraphNode);
	}

	for (const auto& AbilityBoost : BoostData->AbilityAndConditionToActivate)
	{
		if (!AbilityToNode.Contains(AbilityBoost.Key))
		{
			//TODO: Make a warning that ability name in skill boost not exist in skill data
			continue;
		}
		auto ToNode = AbilityToNode[AbilityBoost.Key];
		auto  ExecPin = ToNode->FindPin(ESkillPhase::ESP_None);
		for (const auto& FromAbility : AbilityBoost.Value.AbilityPhaseDependOn)
		{
			auto FromNode = AbilityToNode[FromAbility.Key];
			if (auto FromPin = FromNode->FindPin(FromAbility.Value))
			{
				GetSchema()->TryCreateConnection(FromPin, ExecPin);
			}
		}
	}

	for (const auto& AbilityNode : AbilityToNode)
	{
		AbilityNode.Value->Sync();
	}
}

FGraphAppearanceInfo USipherEdGraph::GetGraphAppearance() const
{
	// Create the appearance info
	FGraphAppearanceInfo AppearanceInfo;

	AppearanceInfo.CornerText = FText::FromString(TEXT("CornerText"));

	AppearanceInfo.InstructionText = FText::FromString(GetName());

	return AppearanceInfo;
}
