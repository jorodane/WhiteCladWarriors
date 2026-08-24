// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/Executables/ActionSelectorNode.h"
#include "Actions/Executables/ActionBehaviorNode.h"
#include "Actions/Executables/ActionExecutor.h"

bool UActionSelectorNode::CompleteInput_Implementation(const FActionCursorFinder& WantCursor)
{
	UActionExecutor* Executor = UActionExecutor::GetExecutorFromID(WantCursor.CurrentExecutorID);
	if (!IsValid(Executor)) return true;
	MoveExecutorToNext(WantCursor);
	return true;
}

void UActionSelectorNode::FailInput_Implementation(const FActionCursorFinder& WantCursor)
{
	UActionExecutor* Executor = UActionExecutor::GetExecutorFromID(WantCursor.CurrentExecutorID);
	if (!IsValid(Executor)) return;
	Executor->EnterNode(WantCursor, BlockedNode, true);
}

bool UActionSelectorNode::CancelInput(const FActionCursorFinder& WantCursor)
{
	UActionExecutor* Executor = UActionExecutor::GetExecutorFromID(WantCursor.CurrentExecutorID);
	if (!IsValid(Executor)) return true;
	if (bCanCancelInput)
	{
		bool Result = OnCancelInput(WantCursor);
		Executor->EnterNode(WantCursor, CanceledNode, true);
		return Result;
	}
	return false;
}

bool UActionSelectorNode::ReceiveInput(const FActionCursorFinder& WantCursor, const FInputPackage& Input)
{
	UActionExecutor* Executor = UActionExecutor::GetExecutorFromID(WantCursor.CurrentExecutorID);
	if (!IsValid(Executor)) return false;
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
			if(IsValid(Input.MouseHitActor)) Result = OnReceiveActor(WantCursor, CurrentTag, Input.MouseHitActor);
			else if(IsValid(Input.MouseClickActor)) Result = OnReceiveActor(WantCursor, CurrentTag, Input.MouseClickActor);
			break;
		case EInputType::MultiTarget:
			//Result = OnReceiveActorArray(WantCursor, CurrentTag, Input.SelectedActors);
			break;
		}

		if (Result)
		{
			Executor->EnterNode(WantCursor, CurrentInputType.OnInputAccepted, false);
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
	for (UActionBehaviorNode* CurrentNode : IndicatorNodes)
	{
		Result.Append(CurrentNode->GetIndicatorClaim(TargetInput));
	}
	return Result;
}