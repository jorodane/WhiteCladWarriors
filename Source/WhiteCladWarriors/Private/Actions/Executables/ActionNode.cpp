// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/Executables/ActionNode.h"
#include "Actions/Executables/ActionExecutor.h"
#include "Objects/Selectables/Components/UnitActionComponent.h"

bool UActionNode::GetCanEnter_Implementation(const FActionCursorFinder& WantCursor)
{
	return !(bIsMainAction && !WantCursor.CurrentComponent->GetMainActionCancelable());
}

void UActionNode::AddNodeLink_Implementation(FName ResultName, const FLinkedNodeInfo& Destination)
{
	LinkedNodes.Add(ResultName, Destination);
}

void UActionNode::MoveExecutorToLinkedNode_Implementation(const FActionCursorFinder& WantCursor, FName ResultName)
{
	UActionExecutor* Executor = WantCursor.CurrentExecutor;
	if (!IsValid(Executor)) return;
	FLinkedNodeInfo* Result = LinkedNodes.Find(ResultName);
	if (Result)
	{
		FLinkedNodeInfo NodeInfo = *Result;
		Executor->EnterNode(WantCursor, NodeInfo.Node);
	}
	else Executor->EndNode(WantCursor, this);
}

void UActionNode::MoveExecutorToNext_Implementation(const FActionCursorFinder& WantCursor)
{
	UActionExecutor* Executor = WantCursor.CurrentExecutor;
	if (!IsValid(Executor)) return;
	Executor->EnterNode(WantCursor, NextNode);
}

void UActionNode::MoveExecutorToCancel_Implementation(const FActionCursorFinder& WantCursor)
{
	UActionExecutor* Executor = WantCursor.CurrentExecutor;
	if (!IsValid(Executor)) return;
	Executor->EnterNode(WantCursor, CanceledNode);
}

void UActionNode::ClaimCancel_Implementation(const FActionCursorFinder& WantCursor)
{
	MoveExecutorToCancel(WantCursor);
}

void UActionNode::ClaimComplete_Implementation(const FActionCursorFinder& WantCursor)
{
	MoveExecutorToNext(WantCursor);
}

void UActionNode::OnActionMessage_Simple_Implementation(const FActionCursorFinder& WantCursor, const FName& Message)
{
	UActionExecutor* Executor = WantCursor.CurrentExecutor;
	UUnitActionComponent* TargetComponent = WantCursor.CurrentComponent;

	if (!IsValid(Executor) || !IsValid(TargetComponent)) return;
	if (FLinkedNodeInfo* NodeFinder = LinkedNodes.Find(Message))
	{
		FLinkedNodeInfo& NodeInfo = *NodeFinder;

		if (NodeInfo.bIsSubNode)
		{
			int ResultID;
			if (FActiveNodeInfo* ResultInfo = WantCursor.CurrentExecutor->GetCursor(TargetComponent))
			{
				Executor->CreateSubNode(WantCursor, *ResultInfo, this, NodeInfo.Node, ResultID);
			}
		}
		else
		{
			Executor->EnterNode(WantCursor, NodeInfo.Node);
		}
	}
}