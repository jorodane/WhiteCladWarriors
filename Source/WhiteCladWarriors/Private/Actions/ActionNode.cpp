// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/ActionNode.h"
#include "Actions/ActionExecutor.h"
#include "Actions/UnitActionComponent.h"

bool UActionNode::GetCanEnter_Implementation(UActionExecutor* Executor, UUnitActionComponent* TargetComponent)
{
	return !(bIsMainAction && !TargetComponent->GetMainActionCancelable());
}

void UActionNode::AddNodeLink_Implementation(FName ResultName, const FLinkedNodeInfo& Destination)
{
	LinkedNodes.Add(ResultName, Destination);
}

void UActionNode::MoveExecutorToLinkedNode_Implementation(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID, FName ResultName)
{
	FLinkedNodeInfo* Result = LinkedNodes.Find(ResultName);
	if (Result)
	{
		FLinkedNodeInfo NodeInfo = *Result;
		Executor->EnterNode(TargetComponent, ID, NodeInfo.Node);
	}
	else Executor->EndNode(TargetComponent, ID, this);
}

void UActionNode::MoveExecutorToNext_Implementation(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID)
{
	if (IsValid(NextNode)) Executor->EnterNode(TargetComponent, ID, NextNode);
	else Executor->EndNode(TargetComponent, ID, this);
}

void UActionNode::MoveExecutorToCancel_Implementation(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID)
{
	if (IsValid(CanceledNode)) Executor->EnterNode(TargetComponent, ID, CanceledNode);
	else Executor->EndNode(TargetComponent, ID, this);
}



void UActionNode::ClaimCancel_Implementation(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID)
{
	MoveExecutorToCancel(Executor, TargetComponent, ID);
}

void UActionNode::ClaimComplete_Implementation(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID)
{
	MoveExecutorToNext(Executor, TargetComponent, ID);
}

void UActionNode::OnActionMessage_Simple_Implementation(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID, const FName& Message)
{
	if (!IsValid(Executor) || !IsValid(TargetComponent)) return;
	if (FLinkedNodeInfo* NodeFinder = LinkedNodes.Find(Message)) 
	{
		FLinkedNodeInfo& NodeInfo = *NodeFinder;
		if (NodeInfo.bIsSubNode)
		{
			Executor->CreateSubNode(TargetComponent, NodeInfo.Node);
		}
		else
		{
			Executor->EnterNode(TargetComponent, ID, NodeInfo.Node);
		}
	}
}