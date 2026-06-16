// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/Executables/ActionNode.h"
#include "Actions/Executables/ActionExecutor.h"
#include "Objects/Selectables/Components/UnitActionComponent.h"

bool UActionNode::GetCanEnter_Implementation(const FActionCursorFinder& WantCursor)
{
	return !(Settings.bIsMainAction && !WantCursor.CurrentComponent->GetMainActionCancelable());
}

void UActionNode::AddNodeLink_Implementation(FName ResultName, const FLinkedNodeInfo& Destination)
{
	LinkedNodes.Add(ResultName, Destination);
}

void UActionNode::MoveExecutorToLinkedNode_Implementation(const FActionCursorFinder& WantCursor, FName ResultName)
{
	UActionExecutor* Executor = UActionExecutor::GetExecutorFromID(WantCursor.CurrentExecutorID);
	UUnitActionComponent* TargetComponent = WantCursor.CurrentComponent;
	if (!IsValid(Executor) || !IsValid(TargetComponent)) return;
	FLinkedNodeInfo* Result = LinkedNodes.Find(ResultName);
	if (Result)
	{
		FLinkedNodeInfo& NodeInfo = *Result;
		if (NodeInfo.bIsSubNode)
		{
			int ResultID;
			if (FActiveNodeMap* ResultInfo = Executor->GetCursor(TargetComponent))
			{
				Executor->CreateSubNode(WantCursor, *ResultInfo, this, NodeInfo.Node, ResultID);
			}
		}
		else
		{
			OnComplete(WantCursor);
			Executor->EnterNode(WantCursor, NodeInfo.Node);
		}
	}
	//else Executor->EndNode(WantCursor, this);
}

void UActionNode::MoveExecutorToNext_Implementation(const FActionCursorFinder& WantCursor)
{
	UActionExecutor* Executor = UActionExecutor::GetExecutorFromID(WantCursor.CurrentExecutorID);
	if (!IsValid(Executor)) return;
	OnComplete(WantCursor);
	Executor->EnterNode(WantCursor, NextNode);
}

void UActionNode::MoveExecutorToCancel_Implementation(const FActionCursorFinder& WantCursor)
{
	UActionExecutor* Executor = UActionExecutor::GetExecutorFromID(WantCursor.CurrentExecutorID);
	if (!IsValid(Executor)) return;
	OnCancel(WantCursor);
	Executor->EnterNode(WantCursor, CanceledNode);
}

void UActionNode::MoveExecutorToInterrupt_Implementation(const FActionCursorFinder& WantCursor, const FActionCursorFinder& InterruptCursor, UActionNode* InterruptNode)
{
	UActionExecutor* Executor = UActionExecutor::GetExecutorFromID(WantCursor.CurrentExecutorID);
	if (!IsValid(Executor)) return;
	OnInterrupt(WantCursor, InterruptCursor, InterruptNode);
	Executor->EnterNode(WantCursor, CanceledNode);
}

void UActionNode::OnActionMessage_Simple_Implementation(const FActionCursorFinder& WantCursor, const FName& Message)
{
	MoveExecutorToLinkedNode(WantCursor, Message);
}