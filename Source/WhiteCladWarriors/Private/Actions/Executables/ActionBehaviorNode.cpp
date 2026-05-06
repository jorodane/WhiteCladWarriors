// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/Executables/ActionBehaviorNode.h"
#include "Actions/Indicators/ActionIndicatorShowerBase.h"
#include "Objects/Selectables/Components/UnitActionComponent.h"

void UActionBehaviorNode::UpdateIndicatorArray_Implementation(UActionBehaviorNode* TargetNode, const FInputClaim& TargetInput, const FInputPackage& InputPackage, const TArray<AActionIndicatorShowerBase*>& TargetShowers, bool bIsIconPreview)
{
	const TArray<UUnitActionComponent*>& TargetComponents = TargetInput.TargetComponentArray;

	int MaxIndex = FMath::Min(TargetComponents.Num(), TargetShowers.Num());
	for (int i = 0; i < MaxIndex; ++i)
	{
		UUnitActionComponent* CurrentComponent = TargetComponents[i];
		AActionIndicatorShowerBase* CurrentShower = TargetShowers[i];
		UUnitMainComponent* CurrentUnit;
		if (!IsValid(CurrentComponent) || !IsValid(CurrentShower)) continue;
		CurrentComponent->TryGetOwnerUnit(CurrentUnit);
		UpdateIndicatorSingle(TargetNode, TargetInput, InputPackage, CurrentComponent, CurrentShower, CurrentUnit, i, bIsIconPreview);
	}
}