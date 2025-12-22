// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/ActionSelectorNode.h"
#include "Actions/ActionExecutor.h"

UActionSelectorNode::UActionSelectorNode()
{
	AddNodeLink("OnFailed",		OnFailed);
	AddNodeLink("OnCanceled", OnCanceled);
}

bool UActionSelectorNode::CompleteInput_Implementation(UActionExecutor* Executor)
{
	MoveExecutorToNext(Executor);
	return true;
}

void UActionSelectorNode::FailInput_Implementation(UActionExecutor* Executor)
{
	MoveExecutorToLinkedNode(Executor, "OnFailed");
}

bool UActionSelectorNode::CancelInput(UActionExecutor* Executor) 
{ 
	if (bCancelable)
	{
		OnCancelInput(Executor);
		MoveExecutorToLinkedNode(Executor, "OnCanceled");
	}
	return bCancelable;
}
