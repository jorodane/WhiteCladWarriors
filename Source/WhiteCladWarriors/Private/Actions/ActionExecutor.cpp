// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/ActionExecutor.h"
#include "Actions/ActionSelectorNode.h"
#include "Actions/UnitActionComponent.h"
#include "Objects/Players/Operator.h"
#include "Actions/ActionNode.h"


void FActiveNodeInfo::SetNode(UActionNode* Node)
{
	CurrentNode = Node;
	bIsMainNode = IsValid(Node) ? Node->bIsMainAction : false;
}

void UActionExecutor::SetActionMessage_Simple(UUnitActionComponent* From, FName Message)
{
	if (FActiveNodeInfo* ComponentInfo = ComponentMap.Find(From))
	{
		if (ComponentInfo)
		{
			UActionNode* CurrentNode = ComponentInfo->CurrentNode;
			if (IsValid(CurrentNode)) CurrentNode->OnActionMessage_Simple(this, From, Message);
		}
	}
}

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
	if (FActiveNodeInfo* ComponentInfo = ComponentMap.Find(WantComponent))
	{
		UActionSelectorNode* CurrentNode = Cast<UActionSelectorNode>(ComponentInfo->CurrentNode);
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


void UActionExecutor::EnterNode(UUnitActionComponent* TargetComponent, UActionNode* TargetNode, int RecursiveDepth)
{
	bool bIsValidNode = IsValid(TargetNode);
	bool bCanEnter = bIsValidNode ? TargetNode->GetCanEnter(this, TargetComponent) : false;
	UActionNode* OriginNode = nullptr;
	if (FActiveNodeInfo* CurrentInfo = ComponentMap.Find(TargetComponent))
	{
		OriginNode = CurrentInfo->CurrentNode;
		if (!bIsValidNode)
		{
			EndNode(TargetComponent, OriginNode);
			return;
		}
		else if (!bCanEnter)
		{
			if (RecursiveDepth > 0) EnterNode(TargetComponent, TargetNode->BlockedNode, RecursiveDepth - 1);
			else EndNode(TargetComponent, OriginNode);
			return;
		}

		CurrentInfo->SetNode(TargetNode);
	}
	else
	{
		if (!bIsValidNode) return;
		else if (!bCanEnter)
		{
			if (RecursiveDepth > 0) EnterNode(TargetComponent, TargetNode->BlockedNode, RecursiveDepth - 1);
			return;
		}
		ComponentMap.Add(TargetComponent, FActiveNodeInfo(TargetNode));
	}
	bool bIsMainAction = bIsValidNode && TargetNode->bIsMainAction;
	bool bWasMainAction = IsValid(OriginNode) ? OriginNode->bIsMainAction : false;
	if (bIsMainAction && !bWasMainAction) TargetComponent->TrySetMainAction(this, TargetNode->bIsCancelable, TargetNode->bIsStopMovementOnStart);
	if (!bIsMainAction && bWasMainAction) TargetComponent->EndMainAction(this, TargetNode->bIsStopMovementOnEnd);
	TargetNode->ClaimExecute(this, TargetComponent);
}

void UActionExecutor::EndNode(UUnitActionComponent* TargetComponent, UActionNode* OldNode)
{
	if (!IsValid(TargetComponent)) return;
	if(IsValid(OldNode) && OldNode->bIsMainAction) TargetComponent->EndMainAction(this, OldNode->bIsStopMovementOnEnd);
	ComponentMap.Remove(TargetComponent);
	CheckComponentMap();
}

void UActionExecutor::AddComponentToMap(UUnitActionComponent* TargetComponent, UActionNode* StartNode)
{
	if (!IsValid(TargetComponent)) return;
	TargetComponent->OnComponentRemoved.AddDynamic(this, &UActionExecutor::RemoveComponentBaseFromMap);
	ComponentMap.Add(TargetComponent, StartNode);
}

void UActionExecutor::AddComponentBaseToMap(UUnitComponentBase* TargetComponent, UActionNode* StartNode) { AddComponentToMap(Cast<UUnitActionComponent>(TargetComponent), StartNode); }

void UActionExecutor::RemoveComponentFromMap(UUnitActionComponent* TargetComponent)
{
	if(IsValid(TargetComponent)) TargetComponent->OnComponentRemoved.RemoveAll(this);
	ComponentMap.Remove(TargetComponent);
	CheckComponentMap();
}

void UActionExecutor::RemoveComponentBaseFromMap(UUnitComponentBase* TargetComponent) { RemoveComponentFromMap(Cast<UUnitActionComponent>(TargetComponent)); }

void UActionExecutor::CheckComponentMap()
{
	if (ComponentMap.Num() == 0) ConditionalBeginDestroy(); 
}

UActionExecutor* UActionExecutor::CreateExecutor(AOperator* TargetOperator, TArray<UUnitActionComponent*> TargetComponents, UActionNode* StartNode)
{
	if(!IsValid(TargetOperator)) return nullptr;
	TargetComponents.RemoveAll([&](UUnitActionComponent* CurrentComponent)->bool{ return !IsValid(CurrentComponent);});
	if(TargetComponents.Num() == 0) return nullptr;

	UActionExecutor* Result = NewObject<UActionExecutor>(TargetOperator);
	if(!IsValid(Result)) return nullptr;
	Result->Operator = TargetOperator;
	for(UUnitActionComponent* CurrentComponent : TargetComponents) Result->AddComponentToMap(CurrentComponent, StartNode);
	return Result;

	return nullptr;
}
