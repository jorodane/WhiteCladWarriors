// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/ActionSelectorNode.h"
#include "Actions/ActionExecutor.h"

bool UActionSelectorNode::CompleteInput_Implementation(UActionExecutor* Executor)
{
	Executor->EnterNode(OnComplete);
	return true;
}

void UActionSelectorNode::FailInput_Implementation(UActionExecutor* Executor)
{
	Executor->EnterNode(OnFailed);
}

bool UActionSelectorNode::CancelInput(UActionExecutor* Executor) 
{ 
	if (bCancelable)
	{
		OnCancelInput(Executor);
		Executor->EnterNode(OnCanceled);
	}
	return bCancelable;
}
