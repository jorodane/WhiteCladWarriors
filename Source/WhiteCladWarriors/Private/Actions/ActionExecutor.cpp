// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/ActionExecutor.h"
#include "Actions/ActionSelectorNode.h"
#include "Objects/Players/Operator.h"
#include "Actions/ActionNode.h"


void UActionExecutor::SetPosition(FName WantTag, const FVector& WantPosition)
{
	FVector& Setter = PositionMap.FindOrAdd(WantTag);
	Setter = WantPosition;
}

FVector UActionExecutor::GetPosition(FName WantTag) const
{
	const FVector* Result = PositionMap.Find(WantTag);
	if (Result) return *Result;
	else return FVector::ZeroVector;
}

bool UActionExecutor::HasPosition(FName WantTag) const { return PositionMap.Contains(WantTag); }

void UActionExecutor::SetDirection(FName WantTag, UUnitActionComponent* WantUUnitActionComponent, const FVector& WantDirection)
{
	FVector& Setter = DirectionMap.FindOrAdd(TPair<UUnitActionComponent*, FName>(WantUUnitActionComponent, WantTag));
	Setter = WantDirection;
}

FVector UActionExecutor::GetDirection(FName WantTag, UUnitActionComponent* WantUUnitActionComponent) const
{
	const FVector* Result = DirectionMap.Find(TPair<UUnitActionComponent*, FName>(WantUUnitActionComponent, WantTag));
	if (Result) return *Result;
	else return FVector::ZeroVector;
}

bool UActionExecutor::HasDirection(FName WantTag, UUnitActionComponent* WantUUnitActionComponent) const { return DirectionMap.Contains(TPair<UUnitActionComponent*, FName>(WantUUnitActionComponent, WantTag)); }

void UActionExecutor::AddActor(FName WantTag, AActor* WantActor)
{
	ActorMultiMap.AddUnique(WantTag, WantActor);
}

void UActionExecutor::RemoveActor(FName WantTag, AActor* WantActor)
{
	ActorMultiMap.RemoveSingle(WantTag, WantActor);
}

AActor* UActionExecutor::GetActor(FName WantTag) const
{
	AActor* const* Result = ActorMultiMap.Find(WantTag);
	if (Result) return *Result;
	else return nullptr;
}

TArray<AActor*> UActionExecutor::GetActorArray(FName WantTag) const
{
	TArray<AActor*> Result;
	ActorMultiMap.MultiFind(WantTag, Result);
	return Result;
}

bool UActionExecutor::SetInput(UUnitActionComponent* WantComponent, UActionSelectorNode* WantNode, const FInputPackage& WantInput)
{
	if (UActionNode** ComponentFinder = ComponentMap.Find(WantComponent))
	{
		UActionSelectorNode* CurrentNode = Cast<UActionSelectorNode>(*ComponentFinder);
		if (IsValid(CurrentNode) && CurrentNode == WantNode) return WantNode->ReceiveInput(this, WantComponent, WantInput);
	}
	return false;
}

bool UActionExecutor::SetInputArray(TArray<UUnitActionComponent*> WantComponent, UActionSelectorNode* WantNode, const FInputPackage& WantInput)
{
	bool Result = false;
	for (UUnitActionComponent* CurrentComponent : WantComponent) Result |= SetInput(CurrentComponent, WantNode, WantInput);
	return Result;
}


void UActionExecutor::EnterNode(UUnitActionComponent* TargetComponent, UActionNode* TargetNode)
{
	bool bIsValidNode = IsValid(TargetNode);
	UActionNode* OriginNode = nullptr;
	if (UActionNode** Finder = ComponentMap.Find(TargetComponent))
	{
		OriginNode = *Finder;
		if (!bIsValidNode)
		{
			EndNode(TargetComponent, OriginNode);
			return;
		}
		else
		{
			ComponentMap.Emplace(TargetComponent, TargetNode);
		}
	}
	else
	{
		if (!bIsValidNode) return;
		ComponentMap.Add(TargetComponent, TargetNode);
	}

	TargetNode->ClaimExecute(this, TargetComponent);
}

void UActionExecutor::EndNode(UUnitActionComponent* TargetComponent, UActionNode* TargetNode)
{
	ComponentMap.Remove(TargetComponent);
	if (ComponentMap.Num() == 0)
	{
		ConditionalBeginDestroy();
	}
}

UActionExecutor* UActionExecutor::CreateExecutor(AOperator* TargetOperator, TArray<UUnitActionComponent*> TargetComponents)
{
	if(!IsValid(TargetOperator) || TargetComponents.Num() == 0) return nullptr;

	UActionExecutor* Result = NewObject<UActionExecutor>(TargetOperator);
	Result->Operator = TargetOperator;
	for (UUnitActionComponent* Currentcomponent : TargetComponents)
	{
		Result->ComponentMap.Add(Currentcomponent, nullptr);
	}
	return Result;
}
