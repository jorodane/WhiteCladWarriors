// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/ActionBase.h"
#include "Actions/Executables/ActionSelectorNode.h"
#include "Actions/Executables/ActionExecutor.h"
#include "Generals/Structs/ActionInputStructures.h"
#include "Objects/Selectables/Components/UnitMainComponent.h"
#include "Objects/Selectables/Components/UnitActionComponent.h"

FText AActionBase::GetActionNameUIWithKey()
{
	if (ActionHotKey.IsValid())
	{
		return FText::Format<FText, FText>(ActionNameUIWithKeyFormat, ActionNameUI, ActionHotKey.GetDisplayName(false));
	}
	else
	{
		return GetActionNameUI();
	}
}

bool AActionBase::IsRootNodeSelector(UActionSelectorNode*& AsSelectorNode) const
{
	AsSelectorNode = RootNodeAsSelector();
	return IsValid(AsSelectorNode);
}

bool AActionBase::IsExecutable_Implementation(UUnitActionComponent* CurrentTarget) const
{
	if (!IsValid(CurrentTarget)) return false;
	if (UUnitMainComponent* AsUnit = CurrentTarget->OwnerUnit)
	{
		if (!IsValid(AsUnit)) return false;
		if (!AsUnit->GetActionExecutable()) return false;
	}
	return true;
}

bool AActionBase::IsNeedInputForStart(FInputClaim& TriggerInput, const TArray<UUnitActionComponent*>& TargetComponent) const
{
	UActionSelectorNode* RootSelector;
	if (IsRootNodeSelector(RootSelector))
	{
		TriggerInput = RootSelector->GetInputClaim(TargetComponent, this, -1);
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
			FActionCursorFinder MainFinder(this, Operator, -1, CurrentComponent, 0, false);

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

UActionExecutor* AActionBase::ExecuteAction_Implementation(AOperator* TargetOperator, const TArray<UUnitActionComponent*>& TargetComponents, const FExecutorValueMap& DefaultValues)
{
	TArray<UUnitActionComponent*> ClaimedComponents = TargetComponents;
	TWeakObjectPtr<UActionExecutor> NewExecutor;
	if (ClaimedComponents.IsEmpty()) return nullptr;
	ClaimedComponents.RemoveAll([this](UUnitActionComponent* Target) {return IsNotExecutable(Target); });
	if (ClaimedComponents.IsEmpty()) return nullptr;
	if (IsValid(RootNode))
	{
		NewExecutor = UActionExecutor::CreateExecutor(this, TargetOperator, ClaimedComponents, RootNode, DefaultValues);
		if (!NewExecutor.IsValid()) return nullptr;
		for (UUnitActionComponent* CurrentComponent : ClaimedComponents)
		{
			FActionCursorFinder MainFinder(this, TargetOperator, NewExecutor->ExecutorID, CurrentComponent, 0, false);
			RootNode->ClaimExecute(MainFinder);
		}
		return NewExecutor.Get();
	}
	return nullptr;
}

UActionExecutor* AActionBase::ExecuteActionWithInput_Implementation(AOperator* TargetOperator, const TArray<UUnitActionComponent*>& TargetComponents, const FExecutorValueMap& DefaultValues, const FInputPackage& Input)
{
	TArray<UUnitActionComponent*> ClaimedComponents = TargetComponents;
	const FInputPackage& ClaimedInput = Input;
	TWeakObjectPtr<UActionExecutor> NewExecutor = nullptr;
	if (ClaimedComponents.IsEmpty()) return nullptr;
	ClaimedComponents.RemoveAll([this](UUnitActionComponent* Target) {return IsNotExecutable(Target); });
	if (ClaimedComponents.IsEmpty()) return nullptr;
	if (IsValid(RootNode))
	{
		NewExecutor = UActionExecutor::CreateExecutor(this, TargetOperator, ClaimedComponents, RootNode, DefaultValues);
		if (!NewExecutor.IsValid()) return nullptr;
		for (UUnitActionComponent* CurrentComponent : ClaimedComponents)
		{
			FActionCursorFinder MainFinder(this, TargetOperator, NewExecutor->ExecutorID, CurrentComponent, 0, false);
			RootNode->ClaimExecuteWithInput(MainFinder, ClaimedInput);
		}
		return NewExecutor.Get();
	}
	return nullptr;
}

UActionExecutor* AActionBase::ExecuteActionToTarget_Implementation(AOperator* WantOperator, UUnitActionComponent* WantComponent, AActor* TargetActor, const FExecutorValueMap& DefaultValues)
{
	if (!IsValid(WantComponent) || !IsValid(TargetActor)) return nullptr;
	FInputPackage CreatedInput;
	CreatedInput.SelectedActors = { WantComponent->GetOwner() };
	CreatedInput.MouseHitActor = TargetActor;
	CreatedInput.MouseTerrainPosition = TargetActor->GetActorLocation();
	return ExecuteActionWithInput(WantOperator, { WantComponent }, DefaultValues, CreatedInput);
}