// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/ActionExecutor.h"
#include "Actions/ActionSelectorNode.h"
#include "Actions/UnitActionComponent.h"
#include "Objects/Selectables/Units/UnitBase.h"
#include "Objects/Players/Operator.h"
#include "Actions/ActionNode.h"


int FActiveNodeInfo::AddNode(UActionNode* Node)
{
	while (NodeMap.Find(nextID++));
	int Result = nextID - 1;
	NodeMap.Add(Result, Node);
	return Result;
}

UActionNode* FActiveNodeInfo::GetNode(int ID) const 
{ 
	if (UActionNode* const* Result = NodeMap.Find(ID)) return *Result;
	else return nullptr;
}

void FActiveNodeInfo::SetNode(UActionNode* Node, int ID) 
{ 
	UActionNode** CurrentNodeFinder = NodeMap.Find(ID);
	if (CurrentNodeFinder) *CurrentNodeFinder = Node;
	else NodeMap.Add(ID, Node);
}

void FActiveNodeInfo::RemoveID(int ID)
{
	NodeMap.Remove(ID);
}

void UActionExecutor::SetActionMessage_Simple(UUnitActionComponent* From, int ID, FName Message)
{
	UActionNode* CurrentNode = GetNode(From, ID);
	if (IsValid(CurrentNode)) CurrentNode->OnActionMessage_Simple(this, From, ID, Message);
}

void UActionExecutor::SetPosition(FName WantTag, const FVector& WantPosition)
{
	FVector& Setter = PositionMap.FindOrAdd(WantTag);
	Setter = WantPosition;
}

FVector UActionExecutor::GetPosition(UPositionClaimer* Claimer, const UUnitActionComponent* From, int ID) const
{
	if (!IsValid(Claimer)) return FVector::ZeroVector;
	return Claimer->GetPosition(this, From, ID);
}

FVector UActionExecutor::GetStartPosition(UDirectionClaimer* Claimer, UUnitActionComponent* From, int ID) const
{
	if (!IsValid(Claimer)) return FVector::ZeroVector;
	return Claimer->GetStartPosition(this, From, ID);
}

FVector UActionExecutor::GetSavedPosition(FName WantTag, const UUnitActionComponent* From, int ID) const
{
	const FVector* Result = PositionMap.Find(WantTag);
	if (Result) return *Result;
	else return FVector::ZeroVector;
}

TArray<UUnitActionComponent*> UActionExecutor::GetComponentArray() const
{
	TArray<UUnitActionComponent*> Result;
	CursorMap.GetKeys(Result);
	return Result;
}


bool UActionExecutor::HasPosition(FName WantTag) const { return PositionMap.Contains(WantTag); }

void UActionExecutor::SetDirection(FName WantTag, UUnitActionComponent* WantUUnitActionComponent, const FVector& WantDirection)
{
	FVector& Setter = DirectionMap.FindOrAdd(TPair<UUnitActionComponent*, FName>(WantUUnitActionComponent, WantTag));
	Setter = WantDirection;
}

FVector UActionExecutor::GetDirection(UDirectionClaimer* Claimer, UUnitActionComponent* WantUUnitActionComponent, int ID) const
{
	if (!IsValid(Claimer)) return FVector::ZeroVector;
	return Claimer->GetDirection(this, WantUUnitActionComponent, ID);
}

FVector UActionExecutor::GetSavedDirection(FName WantTag, UUnitActionComponent* WantComponent, int ID) const
{
	const FVector* Result = DirectionMap.Find(TPair<UUnitActionComponent*, FName>(WantComponent, WantTag));
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

AActor* UActionExecutor::GetActor(UActorClaimer* Claimer, const UUnitActionComponent* WantComponent, int ID) const
{
	if (!IsValid(Claimer)) return nullptr;
	return Claimer->GetActor(this, WantComponent, ID);
}

AActor* UActionExecutor::GetSavedActor(FName WantTag, const UUnitActionComponent* WantComponent, int ID) const
{
	AActor* const* Result = ActorMultiMap.Find(WantTag);
	if (Result) return *Result;
	else return nullptr;
}

TArray<AActor*> UActionExecutor::GetActorArray(UActorArrayClaimer* Claimer, const UUnitActionComponent* WantComponent, int ID) const
{
	if (!IsValid(Claimer)) return TArray<AActor*>();
	return Claimer->GetActorArray(this, WantComponent, ID);
}

TArray<AActor*> UActionExecutor::GetSavedActorArray(FName WantTag, const UUnitActionComponent* WantComponent, int ID) const
{
	TArray<AActor*> Result;
	ActorMultiMap.MultiFind(WantTag, Result);
	return Result;
}

bool UActionExecutor::SetInput(UUnitActionComponent* WantComponent, int ID, UActionSelectorNode* WantNode, const FInputPackage& WantInput)
{
	if (FActiveNodeInfo* ComponentInfo = CursorMap.Find(WantComponent))
	{
		UActionSelectorNode* CurrentNode = Cast<UActionSelectorNode>(GetNode(WantComponent));
		if (IsValid(CurrentNode) && CurrentNode == WantNode) return WantNode->ReceiveInput(this, WantComponent, ID, WantInput);
	}
	return false;
}

bool UActionExecutor::SetInputArray(TArray<UUnitActionComponent*> WantComponent, int ID, UActionSelectorNode* WantNode, const FInputPackage& WantInput)
{
	bool Result = false;
	for (UUnitActionComponent* CurrentComponent : WantComponent) Result |= SetInput(CurrentComponent, ID, WantNode, WantInput);
	return Result;
}

void UActionExecutor::EnterNode(UUnitActionComponent* TargetComponent, int ID, UActionNode* TargetNode, int RecursiveDepth)
{
	bool bIsValidNode = IsValid(TargetNode);
	bool bCanEnter = bIsValidNode ? TargetNode->GetCanEnter(this, TargetComponent) : false;
	UActionNode* OriginNode = nullptr;
	if (FActiveNodeInfo* CurrentInfo = GetCursor(TargetComponent))
	{
		if (ID < 0) ID = CurrentInfo->AddNode(TargetNode);
		OriginNode = CurrentInfo->GetNode(ID);
		if (!bIsValidNode)
		{
			EndNode(TargetComponent, ID, OriginNode);
			return;
		}
		else if (!bCanEnter)
		{
			if (RecursiveDepth > 0) EnterNode(TargetComponent, ID, TargetNode->BlockedNode, RecursiveDepth - 1);
			else EndNode(TargetComponent, ID, OriginNode);
			return;
		}

		CurrentInfo->SetNode(TargetNode, ID);
	}
	else
	{
		if (!bIsValidNode) return;
		else if (!bCanEnter)
		{
			if (RecursiveDepth > 0) EnterNode(TargetComponent, ID, TargetNode->BlockedNode, RecursiveDepth - 1);
			return;
		}
		CursorMap.Add(TargetComponent, TargetNode);
	}

	bool bIsMainAction = TargetNode->bIsMainAction;
	bool bWasMainAction = IsValid(OriginNode) ? OriginNode->bIsMainAction : false;
	if (bIsMainAction) TargetComponent->TrySetMainAction(this, TargetNode->bIsCancelable, TargetNode->bIsStopMovementOnStart);
	else if (bWasMainAction) TargetComponent->EndMainAction(this, OriginNode->bIsStopMovementOnEnd);

	TargetNode->ClaimExecute(this, TargetComponent, ID);
}

void UActionExecutor::EndNode(UUnitActionComponent* TargetComponent, int ID, UActionNode* OldNode)
{
	if (!IsValid(TargetComponent)) return;
	if (IsValid(OldNode) && OldNode->bIsMainAction) TargetComponent->EndMainAction(this, OldNode->bIsStopMovementOnEnd);
	if (FActiveNodeInfo* CursorFinder = GetCursor(TargetComponent))
	{
		FActiveNodeInfo& Cursor = *CursorFinder;
		Cursor.RemoveID(ID);
		if (Cursor.IsEmpty())
		{
			if (AUnitBase* Unit = TargetComponent->GetOwnerUnit()) Unit->NotifyExecutorEnded(this, TargetComponent);
			CursorMap.Remove(TargetComponent);
			CheckCursorMap();
		}
	}
}

void UActionExecutor::CompleteNode(UUnitActionComponent* TargetComponent, int ID)
{
	UActionNode* WantNode = GetNode(TargetComponent, ID);
	if (!IsValid(WantNode)) return;
	WantNode->ClaimComplete(this, TargetComponent, ID);
}

void UActionExecutor::CancelNode(UUnitActionComponent* TargetComponent, int ID)
{
	UActionNode* WantNode = GetNode(TargetComponent, ID);
	if (!IsValid(WantNode)) return;
	WantNode->ClaimCancel(this, TargetComponent, ID);
}

void UActionExecutor::CancelMainNode(UUnitActionComponent* TargetComponent)
{
	if (UActionNode* WantNode = GetNode(TargetComponent)) WantNode->ClaimCancel(this, TargetComponent, 0);
}

void UActionExecutor::AddComponentToMap(UUnitActionComponent* TargetComponent, UActionNode* StartNode)
{
	if (!IsValid(TargetComponent)) return;
	TargetComponent->OnComponentRemoved.AddDynamic(this, &UActionExecutor::RemoveComponentBaseFromMap);
	CursorMap.Add(TargetComponent, StartNode);
}

void UActionExecutor::AddComponentBaseToMap(UUnitComponentBase* TargetComponent, UActionNode* StartNode) { AddComponentToMap(Cast<UUnitActionComponent>(TargetComponent), StartNode); }

void UActionExecutor::RemoveComponentFromMap(UUnitActionComponent* TargetComponent)
{
	if (IsValid(TargetComponent)) TargetComponent->OnComponentRemoved.RemoveAll(this);
	CursorMap.Remove(TargetComponent);
	CheckCursorMap();
}

void UActionExecutor::RemoveComponentBaseFromMap(UUnitComponentBase* TargetComponent) { RemoveComponentFromMap(Cast<UUnitActionComponent>(TargetComponent)); }

FActiveNodeInfo* UActionExecutor::GetCursor(UUnitActionComponent* TargetComponent)
{
	if (FActiveNodeInfo* ComponentInfo = CursorMap.Find(TargetComponent)) { return ComponentInfo; }
	else return nullptr;
}

UActionNode* UActionExecutor::GetNode(UUnitActionComponent* TargetComponent, int ID)
{
	if (FActiveNodeInfo* Finder = GetCursor(TargetComponent)) return Finder->GetNode(ID);
	return nullptr;
}

void UActionExecutor::CheckCursorMap()
{
	if (CursorMap.Num() == 0 && CreatedActors.Num() == 0)
	{
		ConditionalBeginDestroy();
	}
}

UActionExecutor* UActionExecutor::CreateExecutor(AOperator* TargetOperator, TArray<UUnitActionComponent*> TargetComponents, UActionNode* StartNode)
{
	if(!IsValid(TargetOperator)) return nullptr;
	TargetComponents.RemoveAll([&](UUnitActionComponent* CurrentComponent)->bool{ return !IsValid(CurrentComponent);});
	if(TargetComponents.Num() == 0) return nullptr;

	UActionExecutor* Result = NewObject<UActionExecutor>(TargetOperator);
	if(!IsValid(Result)) return nullptr;
	Result->Operator = TargetOperator;
	for (UUnitActionComponent* CurrentComponent : TargetComponents)
	{
		if (!IsValid(CurrentComponent)) continue;
		Result->AddComponentToMap(CurrentComponent, StartNode);
		//if (IsValid(StartNode)) StartNode->ClaimExecute(Result, CurrentComponent);
	}
	return Result;

	return nullptr;
}
