// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/ActionSelectorNode.h"
#include "Actions/ActionExecutor.h"

UActionSelectorNode::UActionSelectorNode()
{
	AddNodeLink("OnFailed",		OnFailed);
	AddNodeLink("OnCanceled", OnCanceled);
}

bool UActionSelectorNode::CompleteInput_Implementation(UActionExecutor* Executor, UUnitActionComponent* TargetComponent)
{
	if (!IsValid(Executor)) return true;
	MoveExecutorToNext(Executor, TargetComponent);
	return true;
}

void UActionSelectorNode::FailInput_Implementation(UActionExecutor* Executor, UUnitActionComponent* TargetComponent)
{
	if (!IsValid(Executor)) return;
	MoveExecutorToLinkedNode(Executor, TargetComponent, "OnFailed");
}

bool UActionSelectorNode::CancelInput(UActionExecutor* Executor, UUnitActionComponent* TargetComponent)
{ 
	if (!IsValid(Executor)) return true;
	if (bCancelable)
	{
		OnCancelInput(Executor);
		MoveExecutorToLinkedNode(Executor, TargetComponent, "OnCanceled");
	}
	return bCancelable;
}