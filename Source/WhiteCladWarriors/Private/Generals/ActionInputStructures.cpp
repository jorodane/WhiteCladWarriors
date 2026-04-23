
#include "Generals/Structs/ActionInputStructures.h"
#include "Objects/Selectables/Components/UnitActionComponent.h"
#include "Objects/Selectables/Components/UnitMainComponent.h"

void FInputClaim::Clear()
{
	TargetNode = nullptr;
	TargetActionCursor.Clear();
	TargetComponentArray.SetNum(0);
	TargetMouseCursorDescription = TargetDescription = FText::GetEmpty();
	TargetMouseCursorType = EInputMouseCursorType::Default;
	TargetReadyMontage.Clear();
}

void FInputClaim::BroadcastStart()
{
	if (TargetComponentArray.IsEmpty()) return;
	for (UUnitActionComponent* CurrentComponent : TargetComponentArray)
	{
		if (!IsValid(CurrentComponent)) continue;
		CurrentComponent->OnInputStart(*this);
	}
}

void FInputClaim::BroadcastEnd()
{
	if (TargetComponentArray.IsEmpty()) return;
	for (UUnitActionComponent* CurrentComponent : TargetComponentArray)
	{
		if(!IsValid(CurrentComponent)) continue;
		CurrentComponent->OnInputEnd(*this);
	}
}

