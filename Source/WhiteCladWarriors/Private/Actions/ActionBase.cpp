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

UActionExecutor* AActionBase::ExecuteAction_Implementation(AOperator* TargetOperator, const TArray<UUnitActionComponent*>& TargetComponents) const
{
	UActionExecutor* NewExecutor = nullptr;
	if (IsValid(RootNode))
	{
		NewExecutor = UActionExecutor::CreateExecutor(TargetOperator, TargetComponents, RootNode);
		for (UUnitActionComponent* CurrentComponent : TargetComponents)
		{
			RootNode->ClaimExecute(NewExecutor, CurrentComponent, 0);
		}
	}
	return NewExecutor;
}
UActionExecutor* AActionBase::ExecuteActionWithInput_Implementation(AOperator* TargetOperator, const TArray<UUnitActionComponent*>& TargetComponents, const FInputPackage& Input) const
{
	UActionExecutor* NewExecutor = nullptr;
	if (IsValid(RootNode))
	{
		NewExecutor = UActionExecutor::CreateExecutor(TargetOperator, TargetComponents, RootNode);
		for (UUnitActionComponent* CurrentComponent : TargetComponents)
		{
			RootNode->ClaimExecuteWithInput(NewExecutor, CurrentComponent, 0, Input);
		}
	}
	return NewExecutor;
}