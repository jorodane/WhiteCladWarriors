// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/ActionSelectorNode.h"
#include "Actions/ActionBehaviorNode.h"
#include "Actions/ActionExecutor.h"

bool UActionSelectorNode::CompleteInput_Implementation(const FActionCursorFinder& WantCursor)
{
	UActionExecutor* Executor = WantCursor.CurrentExecutor;
	if (!IsValid(Executor)) return true;
	MoveExecutorToNext(WantCursor);
	return true;
}

void UActionSelectorNode::FailInput_Implementation(const FActionCursorFinder& WantCursor)
{
	UActionExecutor* Executor = WantCursor.CurrentExecutor;
	if (!IsValid(Executor)) return;
	Executor->EnterNode(WantCursor, BlockedNode);
}

bool UActionSelectorNode::CancelInput(const FActionCursorFinder& WantCursor)
{ 
	UActionExecutor* Executor = WantCursor.CurrentExecutor;
	if (!IsValid(Executor)) return true;
	if (bIsCancelable)
	{
		OnCancelInput(WantCursor);
		Executor->EnterNode(WantCursor, CanceledNode);
	}
	return bIsCancelable;
}

bool UActionSelectorNode::ReceiveInput(const FActionCursorFinder& WantCursor, const FInputPackage& Input)
{ 
	UActionExecutor* Executor = WantCursor.CurrentExecutor;
	bool Result = false;
	for (const FSelectorInput& CurrentInputType : InputTypes)
	{
		FName CurrentTag = CurrentInputType.Tag;
		switch (CurrentInputType.Type)
		{
		case EInputType::Position:
			Result = OnReceivePosition(WantCursor, CurrentTag, Input.MouseTerrainPosition);
			break;
		case EInputType::Direction:
			Result = OnReceiveDirection(WantCursor, CurrentTag, Input.MouseTerrainPosition);
			break;
		case EInputType::SingleTarget:
			Result = OnReceiveActor(WantCursor, CurrentTag, Input.MouseHitActor);
			break;
		case EInputType::MultiTarget:
			//Result = OnReceiveActorArray(WantCursor, CurrentTag, Input.SelectedActors);
			break;
		}

		if (Result)
		{
			Executor->EnterNode(WantCursor, CurrentInputType.OnInputAccepted);
			break;
		}
	}

	Result |= OnReceiveInput(WantCursor, Input);

	return Result;
}

bool UActionSelectorNode::CheckInput(const FActionCursorFinder& WantCursor, const FInputPackage& Input, EInputType& ResultType, FText& FailReason)
{
	bool Result = false;
	FailReason = InputFailReason;
	for (const FSelectorInput& CurrentInputType : InputTypes)
	{
		FName CurrentTag = CurrentInputType.Tag;
		EInputType CurrentType = CurrentInputType.Type;

		switch (CurrentInputType.Type)
		{
		case EInputType::Position:
			Result = CheckPosition(WantCursor, CurrentTag, Input.MouseTerrainPosition);
			break;
		case EInputType::Direction:
			Result = CheckDirection(WantCursor, CurrentTag, Input.MouseTerrainPosition);
			break;
		case EInputType::SingleTarget:
			Result = CheckActor(WantCursor, CurrentTag, Input.MouseHitActor);
			break;
		case EInputType::MultiTarget:
			//Result = OnReceiveActorArray(WantCursor, CurrentTag, Input.SelectedActors);
			break;
		}

		if (Result)
		{
			ResultType = CurrentType;
			break;
		}
	}
	return Result;
}

TArray<UActionBehaviorNode*> UActionSelectorNode::GetIndicatorNodes()
{
	return IndicatorNodes;
}

TMap<UActionBehaviorNode*, FIndicatorClaim> UActionSelectorNode::GetIndicatorClaim(const FInputClaim& TargetInput)
{
	TMap<UActionBehaviorNode*, FIndicatorClaim> Result;
	for (UActionBehaviorNode* CurrentNode : IndicatorNodes) Result.Add(CurrentNode, CurrentNode->GetIndicatorClaim(TargetInput));
	return Result;
}