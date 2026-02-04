// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/ActionSelectorNode.h"
#include "Actions/ActionExecutor.h"

bool UActionSelectorNode::CompleteInput_Implementation(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID)
{
	if (!IsValid(Executor)) return true;
	MoveExecutorToNext(Executor, TargetComponent, ID);
	return true;
}

void UActionSelectorNode::FailInput_Implementation(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID)
{
	if (!IsValid(Executor)) return;
	Executor->EnterNode(TargetComponent, ID, BlockedNode);
}

bool UActionSelectorNode::CancelInput(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID)
{ 
	if (!IsValid(Executor)) return true;
	if (bIsCancelable)
	{
		OnCancelInput(Executor, TargetComponent, ID);
		Executor->EnterNode(TargetComponent, ID, CanceledNode);
	}
	return bIsCancelable;
}

bool UActionSelectorNode::ReceiveInput(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID, const FInputPackage& Input)
{ 
	bool Result = false;
	for (const FSelectorInput& CurrentInputType : InputTypes)
	{
		FName CurrentTag = CurrentInputType.Tag;
		switch (CurrentInputType.Type)
		{
		case EInputType::Position:
			Result = OnReceivePosition(Executor, TargetComponent, ID, CurrentTag, Input.MouseTerrainPosition);
			break;
		case EInputType::Direction:
			Result = OnReceiveDirection(Executor, TargetComponent, ID, CurrentTag, Input.MouseTerrainPosition);
			break;
		case EInputType::SingleTarget:
			Result = OnReceiveActor(Executor, TargetComponent, ID, CurrentTag, Input.MouseHitActor);
			break;
		case EInputType::MultiTarget:
			//Result = OnReceiveActorArray(Executor, TargetComponent, ID, CurrentTag, Input.SelectedActors);
			break;
		}
		if (Result)
		{
			Executor->EnterNode(TargetComponent, ID, CurrentInputType.OnInputAccepted);
			break;
		}
	}

	Result |= OnReceiveInput(Executor, TargetComponent, ID, Input);

	return Result;
}

bool UActionSelectorNode::CheckInput(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID, const FInputPackage& Input, EInputType& ResultType, FText& FailReason)
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
			Result = CheckPosition(Executor, TargetComponent, ID, CurrentTag, Input.MouseTerrainPosition);
			break;
		case EInputType::Direction:
			Result = CheckDirection(Executor, TargetComponent, ID, CurrentTag, Input.MouseTerrainPosition);
			break;
		case EInputType::SingleTarget:
			Result = CheckActor(Executor, TargetComponent, ID, CurrentTag, Input.MouseHitActor);
			break;
		case EInputType::MultiTarget:
			//Result = OnReceiveActorArray(Executor, TargetComponent, ID, CurrentTag, Input.SelectedActors);
			break;
		}

		if (Result)
		{
			ResultType = CurrentType;
			break;
		}
	}

	OnCheckEffectSpawn(Result, Executor, TargetComponent, ID, Input, ResultType, FailReason);

	return Result;
}