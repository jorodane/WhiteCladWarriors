// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/ActionSelectorNode.h"
#include "Actions/ActionExecutor.h"

bool UActionSelectorNode::CompleteInput_Implementation(UActionExecutor* Executor, UUnitActionComponent* TargetComponent)
{
	if (!IsValid(Executor)) return true;
	MoveExecutorToNext(Executor, TargetComponent);
	return true;
}

void UActionSelectorNode::FailInput_Implementation(UActionExecutor* Executor, UUnitActionComponent* TargetComponent)
{
	if (!IsValid(Executor)) return;
	Executor->EnterNode(TargetComponent, BlockedNode);
}

bool UActionSelectorNode::CancelInput(UActionExecutor* Executor, UUnitActionComponent* TargetComponent)
{ 
	if (!IsValid(Executor)) return true;
	if (bIsCancelable)
	{
		OnCancelInput(Executor);
		Executor->EnterNode(TargetComponent, CanceledNode);
	}
	return bIsCancelable;
}