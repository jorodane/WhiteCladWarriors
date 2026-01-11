// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/ActionSelectorNode.h"
#include "Actions/ActionExecutor.h"

bool UActionSelectorNode::CompleteInput_Implementation(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID)
{
	if (!IsValid(Executor)) return true;
	MoveExecutorToNext(Executor, TargetComponent, ID);
	return true;
}

void UActionSelectorNode::FailInput_Implementation(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID)
{
	if (!IsValid(Executor)) return;
	Executor->EnterNode(TargetComponent, ID, BlockedNode);
}

bool UActionSelectorNode::CancelInput(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID)
{ 
	if (!IsValid(Executor)) return true;
	if (bIsCancelable)
	{
		OnCancelInput(Executor, TargetComponent, ID);
		Executor->EnterNode(TargetComponent, ID, CanceledNode);
	}
	return bIsCancelable;
}