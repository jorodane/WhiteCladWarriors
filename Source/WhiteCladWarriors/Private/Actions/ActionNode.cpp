// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/ActionNode.h"
#include "Actions/ActionExecutor.h"
#include "Actions/UnitActionComponent.h"

bool UActionNode::GetCanEnter_Implementation(UActionExecutor* Executor, UUnitActionComponent* TargetComponent)
{
	return !(bIsMainAction && !TargetComponent->GetMainActionCancelable());
}

void UActionNode::AddNodeLink_Implementation(FName ResultName, UActionNode* Destination)
{
	LinkedNodes.Add(ResultName, Destination);
}

void UActionNode::MoveExecutorToLinkedNode_Implementation(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, FName ResultName)
{
	UActionNode** Result = LinkedNodes.Find(ResultName);
	if (IsValid(*Result)) Executor->EnterNode(TargetComponent, *Result);
	else Executor->EndNode(TargetComponent, this);
}

void UActionNode::MoveExecutorToNext_Implementation(UActionExecutor* Executor, UUnitActionComponent* TargetComponent)
{
	if (IsValid(NextNode)) Executor->EnterNode(TargetComponent, NextNode);
	else Executor->EndNode(TargetComponent, this);
}

void UActionNode::ClaimCancel_Implementation(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, bool bWantStopMovement)
{
	if (IsValid(CanceledNode)) Executor->EnterNode(TargetComponent, CanceledNode);
	else Executor->EndNode(TargetComponent, this);
}

void UActionNode::OnActionMessage_Simple_Implementation(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, const FName& Message)
{
	if (!IsValid(Executor) || !IsValid(TargetComponent)) return;
	if (UActionNode** NodeFinder = LinkedNodes.Find(Message)) Executor->EnterNode(TargetComponent, *NodeFinder);
}