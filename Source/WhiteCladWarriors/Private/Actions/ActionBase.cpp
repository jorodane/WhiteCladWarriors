// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/ActionBase.h"
#include "Actions/ActionSelectorNode.h"
#include "Actions/ActionExecutor.h"
#include "Generals/Structs/ActionInputStructures.h"
#include "Objects/Selectables/Components/UnitActionComponent.h"

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

bool AActionBase::IsNeedInputForStartCheck() const
{
	return IsValid(RootNodeAsSelector());
}

bool AActionBase::IsValidInputForStart(const FInputPackage& Input, AOperator* Operator, const TArray<UUnitActionComponent*>& TargetComponent, TArray<bool>& ResultEachComponent, EInputType& TypeResult, FText& ReasonResult)
{
	
	int amount = TargetComponent.Num();
	int index = 0;
	bool Result = false;
	if (amount == 0) return Result;
	ResultEachComponent = TArray<bool>(&Result, amount);
	for (UUnitActionComponent* CurrentComponent : TargetComponent)
	{
		if (IsValid(CurrentComponent))
		{
			FActionCursorFinder MainFinder(this, Operator, nullptr, CurrentComponent, 0);

			if (UActionSelectorNode* RootSelector = RootNodeAsSelector())
			{
				Result |= ResultEachComponent[index] = RootSelector->CheckInput(MainFinder, Input, TypeResult, ReasonResult);
			}
		}
		index++;
	}
	return Result;
}

UActionSelectorNode* AActionBase::RootNodeAsSelector() const
{
	return Cast<UActionSelectorNode>(RootNode);
}

UActionExecutor* AActionBase::ExecuteAction_Implementation(AOperator* TargetOperator, const TArray<UUnitActionComponent*>& TargetComponents)
{
	UActionExecutor* NewExecutor = nullptr;
	if (IsValid(RootNode))
	{
		NewExecutor = UActionExecutor::CreateExecutor(TargetOperator, TargetComponents, RootNode);
		for (UUnitActionComponent* CurrentComponent : TargetComponents)
		{
			FActionCursorFinder MainFinder(this, TargetOperator, NewExecutor, CurrentComponent, 0);
			RootNode->ClaimExecute(MainFinder);
		}
	}
	return NewExecutor;
}

UActionExecutor* AActionBase::ExecuteActionWithInput_Implementation(AOperator* TargetOperator, const TArray<UUnitActionComponent*>& TargetComponents, const FInputPackage& Input)
{
	UActionExecutor* NewExecutor = nullptr;
	if (IsValid(RootNode))
	{
		NewExecutor = UActionExecutor::CreateExecutor(TargetOperator, TargetComponents, RootNode);
		for (UUnitActionComponent* CurrentComponent : TargetComponents)
		{
			FActionCursorFinder MainFinder(this, TargetOperator, NewExecutor, CurrentComponent, 0);
			RootNode->ClaimExecuteWithInput(MainFinder, Input);
		}
	}
	return NewExecutor;
}