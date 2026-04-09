// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/Executables/ActionExecutor.h"
#include "Actions/Executables/ActionSelectorNode.h"
#include "Objects/Selectables/Components/UnitActionComponent.h"
#include "Objects/Selectables/Components/UnitMainComponent.h"
#include "Objects/Players/Operator.h"


int FActiveNodeMap::AddNode(UActionNode* Node)
{
	while (NodeMap.Find(nextID++));
	int Result = nextID - 1;
	NodeMap.Add(Result, Node);
	return Result;
}

UActionNode* FActiveNodeMap::GetNode(int ID)
{
	if (const FActiveNodeInfo* CurrentInfo = GetInfo(ID)) return CurrentInfo->CurrentNode;
	else return nullptr;
}

FActiveNodeInfo* FActiveNodeMap::GetInfo(int ID)
{
	if (NodeMap.IsEmpty()) return nullptr;
	if (FActiveNodeInfo* Result = NodeMap.Find(ID)) return Result;
	else return nullptr;
}

FActiveNodeInfo& FActiveNodeMap::SetNode(UActionNode* Node, int ID)
{
	FActiveNodeInfo* CurrentNodeFinder = NodeMap.Find(ID);
	if (CurrentNodeFinder) return *CurrentNodeFinder = Node;
	else return NodeMap.Add(ID, Node);
}

void FActiveNodeMap::RemoveID(int ID)
{
	NodeMap.Remove(ID);
}

void FActiveNodeMap::BroadcastMessage(const FActionCursorFinder& Cursor, FName Message)
{
	if (IsEmpty()) return;
	TMap<int, FActiveNodeInfo> ReceivedNodeMap = NodeMap;

	for (const auto& CurrentPair : ReceivedNodeMap)
	{
		const FActiveNodeInfo& CurrentInfo = CurrentPair.Value;
		if (CurrentInfo.CurrentListeningState != ENodeListeningState::Listening) continue;

		if (UActionNode* CurrentNode = CurrentInfo.CurrentNode)
		{
			FActionCursorFinder NewCursor = Cursor;
			NewCursor.CurrentID = CurrentPair.Key;
			CurrentNode->OnActionMessage_Simple(NewCursor, Message);
		}
	}
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
	if (FActiveNodeMap* ComponentInfo = CursorMap.Find(WantCursor.CurrentComponent))
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
	FActiveNodeInfo* Result = nullptr;
	if (FActiveNodeMap* CurrentInfo = GetCursor(TargetComponent))
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

		Result = &CurrentInfo->SetNode(TargetNode, ID);
		if (Result) Result->TryListeningPending();
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
		Result = CursorMap.Add(TargetComponent, TargetNode).GetInfo(0);
	}
	const bool bIsMainAction = IsValid(TargetNode) ? TargetNode->bIsMainAction : false;
	const bool bWasMainAction = IsValid(OriginNode) ? OriginNode->bIsMainAction : false;

	const bool bEnterMainLine =  bIsMainAction && !bWasMainAction;
	const bool bExitMainLine  = !bIsMainAction &&  bWasMainAction;

	if (bExitMainLine) TargetComponent->EndMainAction(this, OriginNode->bIsStopMovementOnEnd);
	if (bEnterMainLine) TargetComponent->TrySetMainAction(WantCursor, TargetNode->bIsCancelable, TargetNode->bIsStopMovementOnStart);
	TargetNode->ClaimExecute(WantCursor);
	if (Result) Result->TryListeningStart();
}

UActionNode* UActionExecutor::CreateSubNode(FActionCursorFinder BaseCursor, FActiveNodeMap& TargetInfo, UActionNode* OriginNode, UActionNode* TargetNode, int& ResultID)
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
	if (FActiveNodeMap* CursorFinder = GetCursor(TargetComponent))
	{
		FActiveNodeMap& Cursor = *CursorFinder;
		Cursor.RemoveID(ID);
		if (Cursor.IsEmpty())
		{
			if (UUnitMainComponent* Unit = TargetComponent->GetOwnerUnit()) Unit->NotifyExecutorEnded(Executor, TargetComponent);
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
	TargetComponent->OnComponentRemoved.AddUniqueDynamic(this, &UActionExecutor::RemoveComponentBaseFromMap);
	TargetComponent->OnComponentMessage_Simple.AddUniqueDynamic(this, &UActionExecutor::OnMessageFromComponent_Simple);
	CursorMap.Add(TargetComponent, StartNode);
}

void UActionExecutor::AddComponentBaseToMap(UUnitComponentBase* TargetComponent, UActionNode* StartNode) { AddComponentToMap(Cast<UUnitActionComponent>(TargetComponent), StartNode); }

void UActionExecutor::RemoveComponentFromMap(UUnitActionComponent* TargetComponent)
{
	if (IsValid(TargetComponent))
	{
		TargetComponent->OnComponentRemoved.RemoveAll(this);
		TargetComponent->OnComponentMessage_Simple.RemoveAll(this);
	}
	CursorMap.Remove(TargetComponent);
	CheckCursorMap();
}

void UActionExecutor::RemoveComponentBaseFromMap(UUnitComponentBase* TargetComponent) { RemoveComponentFromMap(Cast<UUnitActionComponent>(TargetComponent)); }

void UActionExecutor::CheckCursorMap()
{
	if (CursorMap.IsEmpty() && CreatedActors.IsEmpty())
	{
		DestroyExecutor(this);
	}
}

FActionCursorFinder UActionExecutor::CreateCursorFinder(UUnitActionComponent* TargetComponent, int TargetID)
{
	return FActionCursorFinder(Action, Operator, this, TargetComponent, TargetID);
}

FActiveNodeMap* UActionExecutor::GetCursor(UUnitActionComponent* TargetComponent)
{
	if (&CursorMap == nullptr || CursorMap.IsEmpty()) return nullptr;
	else if (FActiveNodeMap* ComponentInfo = CursorMap.Find(TargetComponent)) { return ComponentInfo; }
	else return nullptr;
}

UActionNode* UActionExecutor::GetNode(const FActionCursorFinder& WantCursor)
{
	if (WantCursor.CheckValid())return nullptr;
	UUnitActionComponent* TargetComponent = WantCursor.CurrentComponent;
	int ID = WantCursor.CurrentID;
	if (FActiveNodeMap* Finder = GetCursor(TargetComponent)) return Finder->GetNode(ID);
	return nullptr;
}

UActionNode* UActionExecutor::GetNode(UUnitActionComponent* TargetComponent, int TargetID)
{
	if (FActiveNodeMap* Finder = GetCursor(TargetComponent)) return Finder->GetNode(TargetID);
	return nullptr;
}



void UActionExecutor::OnMessageFromComponent_Simple(UUnitComponentBase* From, FName Message)
{
	if (UUnitActionComponent* AsActionComponent = Cast<UUnitActionComponent>(From))
	{
		FActionCursorFinder BaseFinder = CreateCursorFinder(AsActionComponent);

		if (FActiveNodeMap* CurrentCursor = GetCursor(AsActionComponent))
		{
			CurrentCursor->BroadcastMessage(BaseFinder, Message);
		}
	}
}


UActionExecutor* UActionExecutor::CreateExecutor(AActionBase* TargetAction, AOperator* TargetOperator, TArray<UUnitActionComponent*> TargetComponents, UActionNode* StartNode)
{
	if(!IsValid(TargetOperator)) return nullptr;
	TargetComponents.RemoveAll([&](UUnitActionComponent* CurrentComponent)->bool{ return !IsValid(CurrentComponent);});
	if(TargetComponents.Num() == 0) return nullptr;

	UActionExecutor* Result = NewObject<UActionExecutor>(TargetOperator);
	if(!IsValid(Result)) return nullptr;
	Result->Action = TargetAction;
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