#include "SipherSkillEdGraphSchema.h"

const FPinConnectionResponse USipherSkillEdGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
    if (A->Direction != EEdGraphPinDirection::EGPD_Output)
        return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Only can from event pin"));

    if (B->Direction != EEdGraphPinDirection::EGPD_Input || B->GetName() != "Exec")
        return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Only can connect to exec node"));

    return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT("OK!"));
}
