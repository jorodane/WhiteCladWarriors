// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/ActionBehaviorNode.h"
#include "Actions/ActionIndicatorShowerBase.h"
#include "Objects/Selectables/Components/UnitActionComponent.h"

void UActionBehaviorNode::UpdateIndicatorArray_Implementation(const FInputClaim& TargetInput, const FInputPackage& InputPackage, const TArray<AActionIndicatorShowerBase*>& TargetShowers)
{
	const TArray<UUnitActionComponent*>& TargetComponents = TargetInput.TargetComponentArray;

	int MaxIndex = FMath::Min(TargetComponents.Num(), TargetShowers.Num());
	for (int i = 0; i < MaxIndex; ++i)
	{
		UUnitActionComponent* CurrentComponent = TargetComponents[i];
		AActionIndicatorShowerBase* CurrentShower = TargetShowers[i];
		AUnitBase* CurrentUnit;
		if (!IsValid(CurrentComponent) || !IsValid(CurrentShower)) continue;
		CurrentComponent->TryGetOwnerUnit(CurrentUnit);
		UpdateIndicatorSingle(TargetInput, InputPackage, CurrentComponent, CurrentShower, CurrentUnit, i);
	}
}