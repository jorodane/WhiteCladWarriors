// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/ActionNode.h"
#include "Actions/ActionExecutor.h"

void UActionNode::AddNodeLink_Implementation(FName ResultName, UActionNode* Destination)
{
	LinkedNodes.Add(ResultName, Destination);
}

void UActionNode::MoveExecutorToLinkedNode_Implementation(UActionExecutor* Executor, FName ResultName)
{
	UActionNode** Result = LinkedNodes.Find(ResultName);
	if (IsValid(*Result)) Executor->EnterNode(*Result);
	else Executor->EndNode(this);
}

void UActionNode::MoveExecutorToNext_Implementation(UActionExecutor* Executor)
{
	if (IsValid(NextNode)) Executor->EnterNode(NextNode);
	else Executor->EndNode(this);
}