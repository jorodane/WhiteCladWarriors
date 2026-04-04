// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/Executables/ActionExecutor.h"
#include "Actions/Executables/ActionSelectorNode.h"
#include "Objects/Selectables/Components/UnitActionComponent.h"
#include "Objects/Selectables/Units/UnitBase.h"
#include "Objects/Players/Operator.h"


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

void UActionExecutor::SetActionMessage_Simple(const FActionCursorFinder& WantCursor, FName Message)
{
	UActionNode* CurrentNode = GetNode(WantCursor);
	if (IsValid(CurrentNode)) CurrentNode->OnActionMessage_Simple(WantCursor, Message);
}

void UActionExecutor::SetPosition(FName WantTag, const FVector& WantPosition)
{
	FVector& Setter = PositionMap.FindOrAdd(WantTag);
	Setter = WantPosition;
}

FVector UActionExecutor::GetSavedPosition(const FActionCursorFinder& WantCursor, FName WantTag) const
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

void UActionExecutor::SetDirection(FName WantTag, const FActionCursorFinder& WantCursor, const FVector& WantDirection)
{
	FVector& Setter = DirectionMap.FindOrAdd(TPair<UUnitActionComponent*, FName>(WantCursor.CurrentComponent, WantTag));
	Setter = WantDirection;
}

FVector UActionExecutor::GetSavedDirection(const FActionCursorFinder& WantCursor, FName WantTag) const
{
	const FVector* Result = DirectionMap.Find(TPair<UUnitActionComponent*, FName>(WantCursor.CurrentComponent, WantTag));
	if (Result) return *Result;
	else return FVector::ZeroVector;
}

bool UActionExecutor::HasDirection(const FActionCursorFinder& WantCursor, FName WantTag) const { return DirectionMap.Contains(TPair<UUnitActionComponent*, FName>(WantCursor.CurrentComponent, WantTag)); }

void UActionExecutor::AddActor(FName WantTag, AActor* WantActor)
{
	ActorMultiMap.AddUnique(WantTag, WantActor);
}

void UActionExecutor::RemoveActor(FName WantTag, AActor* WantActor)
{
	ActorMultiMap.RemoveSingle(WantTag, WantActor);
}

AActor* UActionExecutor::GetSavedActor(const FActionCursorFinder& WantCursor, FName WantTag) const
{
	AActor* const* Result = ActorMultiMap.Find(WantTag);
	if (Result) return *Result;
	else return nullptr;
}

TArray<AActor*> UActionExecutor::GetSavedActorArray(const FActionCursorFinder& WantCursor, FName WantTag) const
{
	TArray<AActor*> Result;
	ActorMultiMap.MultiFind(WantTag, Result);
	return Result;
}

bool UActionExecutor::SetInput(const FActionCursorFinder& WantCursor, UActionSelectorNode* WantNode, const FInputPackage& WantInput)
{
	if (FActiveNodeInfo* ComponentInfo = CursorMap.Find(WantCursor.CurrentComponent))
	{
		UActionSelectorNode* CurrentNode = Cast<UActionSelectorNode>(GetNode(WantCursor));
		if (IsValid(CurrentNode) && CurrentNode == WantNode) return WantNode->ReceiveInput(WantCursor, WantInput);
	}
	return false;
}

bool UActionExecutor::SetInputArray(TArray<FActionCursorFinder> CursorArray, UActionSelectorNode* WantNode, const FInputPackage& WantInput)
{
	bool Result = false;
	for (const FActionCursorFinder& CurrentCursor : CursorArray) Result |= SetInput(CurrentCursor, WantNode, WantInput);
	return Result;
}

void UActionExecutor::EnterNode(const FActionCursorFinder& WantCursor, UActionNode* TargetNode, int RecursiveDepth)
{
	UUnitActionComponent* TargetComponent = WantCursor.CurrentComponent;
	int ID = WantCursor.CurrentID;
	bool bIsValidNode = IsValid(TargetNode);
	bool bCanEnter = bIsValidNode ? TargetNode->GetCanEnter(WantCursor) : false;
	UActionNode* OriginNode = nullptr;
	if (FActiveNodeInfo* CurrentInfo = GetCursor(TargetComponent))
	{
		OriginNode = CurrentInfo->GetNode(ID);

		if (!bIsValidNode)
		{
			EndNode(WantCursor, OriginNode);
			return;
		}
		else if (!bCanEnter)
		{
			if (RecursiveDepth > 0) EnterNode(WantCursor, TargetNode->BlockedNode, RecursiveDepth - 1);
			else EndNode(WantCursor, OriginNode);
			return;
		}

		CurrentInfo->SetNode(TargetNode, ID);
	}
	else
	{
		if (!bIsValidNode)
		{
			CheckCursorMap();
			return;
		}
		else if (!bCanEnter)
		{
			if (RecursiveDepth > 0) EnterNode(WantCursor, TargetNode->BlockedNode, RecursiveDepth - 1);
			return;
		}
		CursorMap.Add(TargetComponent, TargetNode);
	}

	bool bIsMainAction = TargetNode->bIsMainAction;
	bool bWasMainAction = IsValid(OriginNode) ? OriginNode->bIsMainAction : false;
	if (bWasMainAction) TargetComponent->EndMainAction(this, OriginNode->bIsStopMovementOnEnd);
	else if (bIsMainAction) TargetComponent->TrySetMainAction(WantCursor, TargetNode->bIsCancelable, TargetNode->bIsStopMovementOnStart);

	TargetNode->ClaimExecute(WantCursor);
}

UActionNode* UActionExecutor::CreateSubNode(FActionCursorFinder BaseCursor, FActiveNodeInfo& TargetInfo, UActionNode* OriginNode, UActionNode* TargetNode, int& ResultID)
{ 
	ResultID = TargetInfo.AddNode(OriginNode);
	UActionNode* Result = TargetInfo.GetNode(ResultID);
	BaseCursor.CurrentID = ResultID;
	EnterNode(BaseCursor, TargetNode);
	return Result;
}

void UActionExecutor::EndNode(const FActionCursorFinder& WantCursor, UActionNode* OldNode)
{
	UActionExecutor* Executor = WantCursor.CurrentExecutor;
	UUnitActionComponent* TargetComponent = WantCursor.CurrentComponent;
	int ID = WantCursor.CurrentID;
	if (!IsValid(TargetComponent)) return;
	if (IsValid(OldNode) && OldNode->bIsMainAction) TargetComponent->EndMainAction(Executor, OldNode->bIsStopMovementOnEnd);
	if (FActiveNodeInfo* CursorFinder = GetCursor(TargetComponent))
	{
		FActiveNodeInfo& Cursor = *CursorFinder;
		Cursor.RemoveID(ID);
		if (Cursor.IsEmpty())
		{
			if (AUnitBase* Unit = TargetComponent->GetOwnerUnit()) Unit->NotifyExecutorEnded(Executor, TargetComponent);
			CursorMap.Remove(TargetComponent);
			CheckCursorMap();
		}
	}
}

void UActionExecutor::CompleteNode(const FActionCursorFinder& WantCursor)
{
	UActionNode* WantNode = GetNode(WantCursor);
	if (!IsValid(WantNode)) return;
	WantNode->ClaimComplete(WantCursor);
}

void UActionExecutor::CancelNode(const FActionCursorFinder& WantCursor)
{
	UActionNode* WantNode = GetNode(WantCursor);
	if (!IsValid(WantNode)) return;
	WantNode->ClaimCancel(WantCursor);
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
	if (&CursorMap == nullptr || CursorMap.IsEmpty()) return nullptr;
	else if (FActiveNodeInfo* ComponentInfo = CursorMap.Find(TargetComponent)) { return ComponentInfo; }
	else return nullptr;
}

UActionNode* UActionExecutor::GetNode(const FActionCursorFinder& WantCursor)
{
	UUnitActionComponent* TargetComponent = WantCursor.CurrentComponent;
	int ID = WantCursor.CurrentID;
	if (FActiveNodeInfo* Finder = GetCursor(TargetComponent)) return Finder->GetNode(ID);
	return nullptr;
}

UActionNode* UActionExecutor::GetNode(UUnitActionComponent* TargetComponent, int TargetID)
{
	if (FActiveNodeInfo* Finder = GetCursor(TargetComponent)) return Finder->GetNode(TargetID);
	return nullptr;
}

void UActionExecutor::CheckCursorMap()
{
	if (CursorMap.IsEmpty() && CreatedActors.IsEmpty())
	{
		DestroyExecutor(this);
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

void UActionExecutor::DestroyExecutor(UActionExecutor* TargetExecutor)
{
	if(IsValid(TargetExecutor))	TargetExecutor->ConditionalBeginDestroy();
}