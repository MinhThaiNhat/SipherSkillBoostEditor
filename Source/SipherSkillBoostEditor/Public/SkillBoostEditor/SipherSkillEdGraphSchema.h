// Copyright Ather Labs, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphSchema.h"
#include "SipherSkillEdGraphSchema.generated.h"

/**
 * 
 */
UCLASS()
class SIPHERSKILLBOOSTEDITOR_API USipherSkillEdGraphSchema : public UEdGraphSchema
{
	GENERATED_BODY()
public:
	const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;
};
