// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/ActionBase.h"
#include "Actions/ActionSelectorNode.h"
#include "Actions/ActionExecutor.h"

bool AActionBase::IsRootNodeSelector(UActionSelectorNode*& AsSelectorNode) const
{
	AsSelectorNode = RootNodeAsSelector();
	return IsValid(AsSelectorNode);
}

bool AActionBase::IsNeedInputForStart(FInputClaim& TriggerInput, const TArray<UUnitActionComponent*>& TargetComponent) const
{
	UActionSelectorNode* RootSelector;
	if (IsRootNodeSelector(RootSelector))
	{
		TriggerInput = RootSelector->GetInputClaim(TargetComponent, this, nullptr);
		return true;
	}
	return false;
}


UActionSelectorNode* AActionBase::RootNodeAsSelector() const
{
	return Cast<UActionSelectorNode>(RootNode);
}

void AActionBase::ExecuteAction_Implementation(AOperator* TargetOperator, const TArray<UUnitActionComponent*>& TargetComponents)
{
	UActionExecutor* NewExecutor = UActionExecutor::CreateExecutor(TargetOperator, TargetComponents, RootNode);
	if (IsValid(RootNode)) for (UUnitActionComponent* CurrentComponent : TargetComponents) RootNode->ClaimExecute(NewExecutor, CurrentComponent, 0);
}
void AActionBase::ExecuteActionWithInput_Implementation(AOperator* TargetOperator, const TArray<UUnitActionComponent*>& TargetComponents, const FInputPackage& Input)
{
	UActionExecutor* NewExecutor = UActionExecutor::CreateExecutor(TargetOperator, TargetComponents, RootNode);
	if (IsValid(RootNode)) for (UUnitActionComponent* CurrentComponent : TargetComponents) RootNode->ClaimExecuteWithInput(NewExecutor, CurrentComponent, 0, Input);
}