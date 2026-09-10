// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/Executables/ActionNode.h"
#include "Actions/Executables/ActionExecutor.h"
#include "Objects/Selectables/Components/UnitActionComponent.h"

const FActionExecuteSettingContainer UActionNode::DefaultExecuteSetting;


bool UActionNode::GetCanEnter_Implementation(const FActionCursorFinder& WantCursor)
{
	return true;
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
			if (FActiveNodeMap* ResultInfo = Executor->GetNodeMap(TargetComponent))
			{
				int ResultID;
				FActionCursorFinder ResultCursor;
				Executor->CreateSubNode(WantCursor, this, NodeInfo.Node, ResultID, ResultCursor);
				Executor->EnterNode(ResultCursor, this, false);
			}
		}
		else
		{
			OnComplete(WantCursor);
			Executor->EnterNode(WantCursor, NodeInfo.Node, false);
		}
	}
	//else Executor->EndNode(WantCursor, this);
}

void UActionNode::MoveExecutorToNext_Implementation(const FActionCursorFinder& WantCursor)
{
	UActionExecutor* Executor = UActionExecutor::GetExecutorFromID(WantCursor.CurrentExecutorID);
	if (!IsValid(Executor)) return;
	OnComplete(WantCursor);
	Executor->EnterNode(WantCursor, NextNode, false);
}

void UActionNode::MoveExecutorToCancel_Implementation(const FActionCursorFinder& WantCursor)
{
	UActionExecutor* Executor = UActionExecutor::GetExecutorFromID(WantCursor.CurrentExecutorID);
	if (!IsValid(Executor)) return;
	OnCancel(WantCursor);
	Executor->EnterNode(WantCursor, CanceledNode, true);
}

void UActionNode::MoveExecutorToInterrupt_Implementation(const FActionCursorFinder& WantCursor, const FActionCursorFinder& InterruptCursor, UActionNode* InterruptNode)
{
	UActionExecutor* Executor = UActionExecutor::GetExecutorFromID(WantCursor.CurrentExecutorID);
	if (!IsValid(Executor)) return;
	OnInterrupt(WantCursor, InterruptCursor, InterruptNode);
	Executor->EnterNode(WantCursor, CanceledNode, true);
}

void UActionNode::MoveExecutorToWantNode_Implementation(const FActionCursorFinder& WantCursor, UActionNode* TargetNode, bool bIsCanceled)
{
	UActionExecutor* Executor = UActionExecutor::GetExecutorFromID(WantCursor.CurrentExecutorID);
	if (!IsValid(Executor)) return;
	if (bIsCanceled) OnCancel(WantCursor);
	else			 OnComplete(WantCursor);
	Executor->EnterNode(WantCursor, TargetNode, bIsCanceled);
}

int UActionNode::CreateSubNode_Implementation(const FActionCursorFinder& WantCursor, UActionNode* TargetNode, FActionCursorFinder& ResultCursor)
{
	UActionExecutor* Executor = UActionExecutor::GetExecutorFromID(WantCursor.CurrentExecutorID);
	ResultCursor = WantCursor;
	if (!IsValid(Executor)) return -1;
	int index;
	Executor->CreateSubNode(WantCursor, this, TargetNode, index, ResultCursor);
	return index;
}

void UActionNode::EndAllSubNodes_Implementation(const FActionCursorFinder& WantCursor)
{
	UActionExecutor* Executor = UActionExecutor::GetExecutorFromID(WantCursor.CurrentExecutorID);
	if (!IsValid(Executor)) return;
	Executor->EndSubNode(WantCursor, true);
}

void UActionNode::OnActionMessage_Simple_Implementation(const FActionCursorFinder& WantCursor, const FName& Message)
{
	MoveExecutorToLinkedNode(WantCursor, Message);
}