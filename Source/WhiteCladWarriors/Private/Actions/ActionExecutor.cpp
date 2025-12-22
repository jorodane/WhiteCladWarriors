// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/ActionExecutor.h"
#include "Actions/ActionNode.h"

void UActionExecutor::SetPosition(const FName& WantTag, const FVector& WantPosition)
{
	FVector& Setter = PositionMap.FindOrAdd(WantTag);
	Setter = WantPosition;
}

FVector UActionExecutor::GetPosition(const FName& WantTag) const
{
	const FVector* Result = PositionMap.Find(WantTag);
	if (Result) return *Result;
	else return FVector::ZeroVector;
}

bool UActionExecutor::HasPosition(const FName& WantTag) const { return PositionMap.Contains(WantTag); }

void UActionExecutor::SetDirection(const FName& WantTag, const FVector& WantDirection)
{
	FVector& Setter = DirectionMap.FindOrAdd(WantTag);
	Setter = WantDirection;
}

FVector UActionExecutor::GetDirection(const FName& WantTag) const
{
	const FVector* Result = DirectionMap.Find(WantTag);
	if (Result) return *Result;
	else return FVector::ZeroVector;
}

bool UActionExecutor::HasDirection(const FName& WantTag) const { return DirectionMap.Contains(WantTag); }

void UActionExecutor::AddActor(const FName& WantTag, AActor* WantActor)
{
	ActorMultiMap.AddUnique(WantTag, WantActor);
}

void UActionExecutor::RemoveActor(const FName& WantTag, AActor* WantActor)
{
	ActorMultiMap.RemoveSingle(WantTag, WantActor);
}

AActor* UActionExecutor::GetActor(const FName& WantTag) const
{
	AActor* const* Result = ActorMultiMap.Find(WantTag);
	if (Result) return *Result;
	else return nullptr;
}

TArray<AActor*> UActionExecutor::GetActorArray(const FName& WantTag) const
{
	TArray<AActor*> Result;
	ActorMultiMap.MultiFind(WantTag, Result);
	return Result;
}


void UActionExecutor::EnterNode(UActionNode* TargetNode)
{
	if (IsValid(TargetNode))
	{
		if(IsValid(CurrentNode))	OnNodeMove(CurrentNode, TargetNode);
		else							OnNodeEnter(TargetNode);
		CurrentNode = TargetNode;
		TargetNode->ClaimExecute(this);
	}
	else
	{
		EndNode(CurrentNode);
	}
}

void UActionExecutor::EndNode(UActionNode* TargetNode)
{
	OnNodeEnd(TargetNode);
	ConditionalBeginDestroy();
}
