// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/ActionExecutor.h"
#include "Actions/ActionNode.h"

void UActionExecutor::EnterNode(UActionNode* TargetNode)
{
	if (IsValid(TargetNode))
	{
		if(IsValid(CurrentNode))	OnNodeMove(CurrentNode, TargetNode);
		else							OnNodeEnter(TargetNode);
		CurrentNode = TargetNode;
		TargetNode->ClaimExecute(this);
	}
	else OnNodeEnd(CurrentNode);
}

