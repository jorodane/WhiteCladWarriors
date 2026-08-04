// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/Executables/ActionExecutor.h"
#include "Actions/Executables/ActionSelectorNode.h"
#include "Actions/ActionBase.h"
#include "Objects/Selectables/Components/UnitActionComponent.h"
#include "Objects/Selectables/Components/UnitMainComponent.h"
#include "Objects/Players/Operator.h"
#include "Settings/ActionSetting.h"

const FExecutorValueMap FExecutorValueMap::Default;

void FActiveNodeMap::Clear()
{
	NodeMap.Reset();
	EndEventMap.Reset();
}

int FActiveNodeMap::AddNode(UActionNode* Node)
{
	while (NodeMap.Find(nextID++));
	int Result = nextID - 1;
	NodeMap.Add(Result, Node);
	return Result;
}

int FActiveNodeMap::AddNode(UActionNode* Node, FOnNodeEnded OnNodeEnded)
{
	int Result = AddNode(Node);
	if (Result >= 0) EndEventMap.Add(Result, OnNodeEnded);
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

void FActiveNodeMap::InvokeEndEvent(int ID)
{
	FOnNodeEnded Result;
	if (EndEventMap.RemoveAndCopyValue(ID, Result)) Result.Execute();
}


void FActiveNodeMap::RemoveID(int ID)
{
	NodeMap.Remove(ID);
	EndEventMap.Remove(ID);
}

void FActiveNodeMap::RemoveSubNodes()
{
	FActiveNodeInfo* MainNode = NodeMap.Find(0);
	NodeMap.Reset();
	if (MainNode)
	{
		NodeMap.Add(0, *MainNode);
	}
}

void FActiveNodeMap::BroadcastFunction(const FActionCursorFinder& Cursor, TFunctionRef<void(UActionNode*, const FActionCursorFinder&)> Function)
{
	if (IsEmpty()) return;
	TMap<int, FActiveNodeInfo> ReceivedNodeMap = NodeMap;

	for (const auto& CurrentPair : ReceivedNodeMap)
	{
		const FActiveNodeInfo& CurrentInfo = CurrentPair.Value;
		if (CurrentInfo.CurrentListeningState != ENodeListeningState::Listening) continue;

		UActionNode* CurrentNode = CurrentInfo.CurrentNode;
		if (IsValid(CurrentNode))
		{
			FActionCursorFinder NewCursor = Cursor;
			NewCursor.CurrentID = CurrentPair.Key;
			Function(CurrentNode, NewCursor);
		}
	}
}

void FActiveNodeMap::BroadcastMessage_Simple(const FActionCursorFinder& Cursor, FName Message)
{
	BroadcastFunction(Cursor, 
	[=](UActionNode* CurrentNode, const FActionCursorFinder& NewCursor)->void
	{
		CurrentNode->OnActionMessage_Simple(NewCursor, Message);
	});
}
void FActiveNodeMap::BroadcastMessage_Detail(const FActionCursorFinder& Cursor, FName Message, const FName& Context)
{
	BroadcastFunction(Cursor,
		[=](UActionNode* CurrentNode, const FActionCursorFinder& NewCursor)->void
		{
			CurrentNode->OnActionMessage_Detail(NewCursor, Message, Context);
		});
}
void FActiveNodeMap::BroadcastMessage_Montage(const FActionCursorFinder& Cursor, UAnimMontage* Montage, bool bIsStart, bool bIsInterrupted)
{
	BroadcastFunction(Cursor,
		[=](UActionNode* CurrentNode, const FActionCursorFinder& NewCursor)->void
		{
			CurrentNode->OnActionMessage_Montage(NewCursor, Montage, bIsStart, bIsInterrupted);
		});
}


void FExecutorValueMap::Clear()
{
	FloatMap.Reset();
	PositionMap.Reset();
	DirectionMap.Reset();
	ActorMultiMap.Reset();
}

bool FExecutorValueMap::HasFloat(FName WantTag) const { return FloatMap.Contains(WantTag); }

void FExecutorValueMap::SetFloat(FName WantTag, const float& WantFloat)
{
	float& Setter = FloatMap.FindOrAdd(WantTag);
	Setter = WantFloat;
}

float FExecutorValueMap::GetSavedFloat(const FActionCursorFinder& WantCursor, FName WantTag) const
{
	const float* Result = FloatMap.Find(WantTag);
	if (Result) return *Result;
	else return 0.0f;
}



bool FExecutorValueMap::HasPosition(FName WantTag) const { return PositionMap.Contains(WantTag); }

void FExecutorValueMap::SetPosition(FName WantTag, const FVector& WantPosition)
{
	FVector& Setter = PositionMap.FindOrAdd(WantTag);
	Setter = WantPosition;
}

FVector FExecutorValueMap::GetSavedPosition(const FActionCursorFinder& WantCursor, FName WantTag) const
{
	const FVector* Result = PositionMap.Find(WantTag);
	if (Result) return *Result;
	else return FVector::ZeroVector;
}


bool FExecutorValueMap::HasDirection(const FActionCursorFinder& WantCursor, FName WantTag) const { return DirectionMap.Contains(TPair<UUnitActionComponent*, FName>(WantCursor.CurrentComponent, WantTag)); }

void FExecutorValueMap::SetDirection(FName WantTag, const FActionCursorFinder& WantCursor, const FVector& WantDirection)
{
	FVector& Setter = DirectionMap.FindOrAdd(TPair<UUnitActionComponent*, FName>(WantCursor.CurrentComponent, WantTag));
	Setter = WantDirection;
}

FVector FExecutorValueMap::GetSavedDirection(const FActionCursorFinder& WantCursor, FName WantTag) const
{
	const FVector* Result = DirectionMap.Find(TPair<UUnitActionComponent*, FName>(WantCursor.CurrentComponent, WantTag));
	if (Result) return *Result;
	else return FVector::ZeroVector;
}


void FExecutorValueMap::AddActor(FName WantTag, AActor* WantActor)
{
	ActorMultiMap.AddUnique(WantTag, WantActor);
}

void FExecutorValueMap::RemoveActor(FName WantTag, AActor* WantActor)
{
	ActorMultiMap.RemoveSingle(WantTag, WantActor);
}

AActor* FExecutorValueMap::GetSavedActor(const FActionCursorFinder& WantCursor, FName WantTag) const
{
	AActor* const* Result = ActorMultiMap.Find(WantTag);
	if (Result) return *Result;
	else return nullptr;
}

TArray<AActor*> FExecutorValueMap::GetSavedActorArray(const FActionCursorFinder& WantCursor, FName WantTag) const
{
	TArray<AActor*> Result;
	ActorMultiMap.MultiFind(WantTag, Result);
	return Result;
}




void UActionExecutor::Clear()
{
	Action = nullptr;
	Operator = nullptr;
	CursorMap.Reset();
	CreatedActors.Reset();
	ValueMap.Clear();
	ExecutorID = -1;
}


void UActionExecutor::SetActionMessage_Simple(const FActionCursorFinder& WantCursor, FName Message)
{
	UActionNode* CurrentNode = GetNode(WantCursor);
	if (IsValid(CurrentNode)) CurrentNode->OnActionMessage_Simple(WantCursor, Message);
}

TArray<UUnitActionComponent*> UActionExecutor::GetComponentArray() const
{
	TArray<UUnitActionComponent*> Result;
	CursorMap.GetKeys(Result);
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
	if (!IsValid(TargetComponent)) return;
	int ID = WantCursor.CurrentID;
	bool bIsValidNode = IsValid(TargetNode);
	bool bCanEnter = bIsValidNode ? TargetNode->GetCanEnter(WantCursor) : false;

	while (!bCanEnter && bIsValidNode && RecursiveDepth > 0)
	{
		TargetNode = TargetNode->BlockedNode;
		bIsValidNode = IsValid(TargetNode);
		bCanEnter = bIsValidNode ? TargetNode->GetCanEnter(WantCursor) : false;
		RecursiveDepth--;
	}
	const bool bIsBlocked = !(bIsValidNode && bCanEnter);

	UActionNode* OriginNode = nullptr;
	FActiveNodeMap* CurrentNodeMap = GetNodeMap(TargetComponent);

	if (CurrentNodeMap)
	{
		OriginNode = CurrentNodeMap->GetNode(ID);
		FActiveNodeInfo* OriginInfo = CurrentNodeMap->GetInfo(ID);
		if (OriginInfo) OriginInfo->TryListeningPending();
		if (bIsBlocked)
		{
			EndNode(WantCursor, OriginNode, false);
			return;
		}
	}
	else if (bIsBlocked)
	{
		CheckCursorMap();
		return;
	}

	if (WantCursor.CheckIsMainNode())
	{
		const bool bWasMainAction = IsValid(OriginNode) ? OriginNode->Settings.bIsMainAction : false;
		const bool bIsMainAction = IsValid(TargetNode) ? TargetNode->Settings.bIsMainAction : false;

		const bool bExitMainLine = !bIsMainAction && bWasMainAction;
		const bool bEnterMainLine = bIsMainAction && !bWasMainAction;

		if (bExitMainLine) TargetComponent->EndMainAction(ExecutorID);
		if (bEnterMainLine)
		{
			if (!TargetComponent->TrySetMainAction(WantCursor, TargetNode))
			{
				EndNode(WantCursor, OriginNode, false);
				return;
			}
		}
	}

	FActiveNodeInfo* CurrentNodeInfo = nullptr;
	if (CurrentNodeMap) CurrentNodeInfo = &CurrentNodeMap->SetNode(TargetNode, ID);
	else CurrentNodeInfo = CursorMap.Add(TargetComponent, TargetNode).GetMainInfo();

	if (IsValid(TargetNode)) TargetNode->ClaimExecute(WantCursor);
	if (GetValid())
	{
		CurrentNodeInfo = CurrentNodeMap->GetInfo(ID);
		if (CurrentNodeInfo) CurrentNodeInfo->TryListeningStart();
	}
}

UActionNode* UActionExecutor::InitiateSubNode(FActionCursorFinder& BaseCursor, FActiveNodeMap& TargetInfo, UActionNode* TargetNode, int& ResultID)
{
	UActionNode* Result = TargetInfo.GetNode(ResultID);
	if (!Result) return nullptr;
	BaseCursor.CurrentID = ResultID;
	BaseCursor.bAsSubNode = true;
	EnterNode(BaseCursor, TargetNode, true);
	return Result;
}

UActionNode* UActionExecutor::CreateSubNode(FActionCursorFinder BaseCursor, UActionNode* OriginNode, UActionNode* TargetNode, int& ResultID)
{
	FActiveNodeMap* CurrentNodeMap = GetNodeMap(BaseCursor.CurrentComponent);
	if (CurrentNodeMap) return CreateSubNode(BaseCursor, *CurrentNodeMap, OriginNode, TargetNode, ResultID);
	return nullptr;
}

UActionNode* UActionExecutor::CreateSubNodeWithEvent(FActionCursorFinder BaseCursor, UActionNode* OriginNode, UActionNode* TargetNode, int& ResultID, const FOnNodeEnded& OnNodeEnded)
{
	FActiveNodeMap* CurrentNodeMap = GetNodeMap(BaseCursor.CurrentComponent);
	if (CurrentNodeMap) return CreateSubNodeWithEvent(BaseCursor, *CurrentNodeMap, OriginNode, TargetNode, ResultID, OnNodeEnded);
	return nullptr;
}

UActionNode* UActionExecutor::CreateSubNode(FActionCursorFinder BaseCursor, FActiveNodeMap& TargetInfo, UActionNode* OriginNode, UActionNode* TargetNode, int& ResultID)
{
	ResultID = TargetInfo.AddNode(OriginNode);
	return InitiateSubNode(BaseCursor, TargetInfo, TargetNode, ResultID);
}

UActionNode* UActionExecutor::CreateSubNodeWithEvent(FActionCursorFinder BaseCursor, FActiveNodeMap& TargetInfo, UActionNode* OriginNode, UActionNode* TargetNode, int& ResultID, const FOnNodeEnded& OnNodeEnded)
{
	ResultID = TargetInfo.AddNode(OriginNode, OnNodeEnded);
	return InitiateSubNode(BaseCursor, TargetInfo, TargetNode, ResultID);
}

UActionNode* UActionExecutor::GetNode(const FActionCursorFinder& WantCursor)
{
	UUnitActionComponent* TargetComponent = WantCursor.CurrentComponent;
	if (!IsValid(TargetComponent)) return nullptr;
	if (FActiveNodeMap* CursorFinder = GetNodeMap(TargetComponent))
	{
		FActiveNodeMap& Cursor = *CursorFinder;
		return Cursor.GetNode(WantCursor.CurrentID);
	}
	return nullptr;
}

void UActionExecutor::EndNode(const FActionCursorFinder& WantCursor, UActionNode* OldNode, bool bEndSubNode)
{
	UUnitActionComponent* TargetComponent = WantCursor.CurrentComponent;
	int ID = WantCursor.CurrentID;
	if (!IsValid(TargetComponent)) return;
	if (WantCursor.CheckIsMainNode() && IsValid(OldNode) && OldNode->Settings.bIsMainAction) TargetComponent->EndMainAction(ExecutorID);
	FActiveNodeMap* CursorFinder = GetNodeMap(TargetComponent);
	if (CursorFinder)
	{
		FActiveNodeMap& Cursor = *CursorFinder;
		if (bEndSubNode) EndSubNode(WantCursor, ID);
		Cursor.InvokeEndEvent(ID);
		Cursor.RemoveID(ID);

		if (Cursor.IsEmpty())
		{
			if (UUnitMainComponent* Unit = TargetComponent->GetOwnerUnit()) Unit->NotifyExecutorEnded(ExecutorID, TargetComponent);
			CursorMap.Remove(TargetComponent);
			CheckCursorMap();
		}
	}
}

void UActionExecutor::EndSubNode(const FActionCursorFinder& WantCursor, int exceptID)
{
	FActiveNodeMap* CursorFinder = GetNodeMap(WantCursor.CurrentComponent);
	if (!CursorFinder) return;
	FActiveNodeMap& Cursor = *CursorFinder;
	if (!Cursor.NodeMap.IsEmpty())
	{
		FActionCursorFinder SubCursor = WantCursor;
		TArray<FActiveNodeInfo> DestroyTarget;
		for (const auto& CurrentNode : Cursor.NodeMap)
		{
			int CurrentID = CurrentNode.Key;
			if (CurrentID == 0 || CurrentID == exceptID) continue;
			SubCursor.CurrentID = CurrentID;
			Cursor.InvokeEndEvent(CurrentID);
			DestroyTarget.Add(CurrentNode.Value);
		}
		for (const FActiveNodeInfo& CurrentTarget : DestroyTarget)
		{
			if (IsValid(CurrentTarget.CurrentNode))
			{
				CurrentTarget.CurrentNode->MoveExecutorToCancel(SubCursor);
			}
		}
	}
}

void UActionExecutor::CompleteNode(const FActionCursorFinder& WantCursor)
{
	UActionNode* WantNode = GetNode(WantCursor);
	if (!IsValid(WantNode)) return;
	WantNode->MoveExecutorToNext(WantCursor);
}

void UActionExecutor::CancelNode(const FActionCursorFinder& WantCursor)
{
	UActionNode* WantNode = GetNode(WantCursor);
	if (!IsValid(WantNode)) return;
	WantNode->MoveExecutorToCancel(WantCursor);
}

void UActionExecutor::InterruptNode(const FActionCursorFinder& WantCursor, const FActionCursorFinder& InterruptCursor, UActionNode* InterruptNode)
{
	if (!IsValid(InterruptNode)) return;
	UActionNode* WantNode = GetNode(WantCursor);
	if (!IsValid(WantNode)) return;
	WantNode->MoveExecutorToInterrupt(WantCursor, InterruptCursor, InterruptNode);
}

void UActionExecutor::AddComponentToMap(UUnitActionComponent* TargetComponent, UActionNode* StartNode)
{
	if (!IsValid(TargetComponent)) return;
	TargetComponent->OnComponentRemoved.AddUniqueDynamic(this, &UActionExecutor::RemoveComponentBaseFromMap);
	TargetComponent->OnComponentMessage_Simple.AddUniqueDynamic(this, &UActionExecutor::OnMessageFromComponent_Simple);
	TargetComponent->OnComponentMessage_Detail.AddUniqueDynamic(this, &UActionExecutor::OnMessageFromComponent_Detail);
	TargetComponent->OnComponentMessage_Montage.AddUniqueDynamic(this, &UActionExecutor::OnMessageFromComponent_Montage);
	CursorMap.Add(TargetComponent, StartNode);
}

void UActionExecutor::AddComponentBaseToMap(UUnitComponentBase* TargetComponent, UActionNode* StartNode) { AddComponentToMap(Cast<UUnitActionComponent>(TargetComponent), StartNode); }

void UActionExecutor::RemoveComponentFromMap(UUnitActionComponent* TargetComponent)
{
	if (IsValid(TargetComponent))
	{
		TargetComponent->OnComponentRemoved.RemoveAll(this);
		TargetComponent->OnComponentMessage_Simple.RemoveAll(this);
		TargetComponent->OnComponentMessage_Detail.RemoveAll(this);
		TargetComponent->OnComponentMessage_Montage.RemoveAll(this);
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

FActionCursorFinder UActionExecutor::CreateCursorFinder(UUnitActionComponent* TargetComponent, int TargetID, bool bAsSubNode)
{
	return FActionCursorFinder(Action, Operator, ExecutorID, TargetComponent, TargetID, bAsSubNode);
}

FActiveNodeMap* UActionExecutor::GetNodeMap(UUnitActionComponent* TargetComponent)
{
	if (&CursorMap == nullptr || CursorMap.IsEmpty()) return nullptr;
	else if (FActiveNodeMap* ComponentInfo = CursorMap.Find(TargetComponent)) { return ComponentInfo; }
	else return nullptr;
}

bool UActionExecutor::SetEndEventOnMainCursor(UUnitActionComponent* TargetComponent, const FOnNodeEnded& OnNodeEnded)
{
	FActiveNodeMap* NodeMap = GetNodeMap(TargetComponent);

	if (NodeMap)
	{
		if (NodeMap->EndEventMap.Contains(0)) return false;
		NodeMap->EndEventMap.Add(0, OnNodeEnded);
		return true;
	}

	return false;
}

void UActionExecutor::OnMessageFromComponent_Simple(UUnitComponentBase* From, const FName& Message)
{
	if (UUnitActionComponent* AsActionComponent = Cast<UUnitActionComponent>(From))
	{
		FActionCursorFinder BaseFinder = CreateCursorFinder(AsActionComponent);

		if (FActiveNodeMap* CurrentCursor = GetNodeMap(AsActionComponent))
		{
			CurrentCursor->BroadcastMessage_Simple(BaseFinder, Message);
		}
	}
}

void UActionExecutor::OnMessageFromComponent_Detail(UUnitComponentBase* From, const FName& Message, const FName& Context)
{
	if (UUnitActionComponent* AsActionComponent = Cast<UUnitActionComponent>(From))
	{
		FActionCursorFinder BaseFinder = CreateCursorFinder(AsActionComponent);

		if (FActiveNodeMap* CurrentCursor = GetNodeMap(AsActionComponent))
		{
			CurrentCursor->BroadcastMessage_Detail(BaseFinder, Message, Context);
		}
	}
}

void UActionExecutor::OnMessageFromComponent_Montage(UUnitComponentBase* From, UAnimMontage* Montage, bool bIsStart, bool bIsInterrupted)
{
	if (UUnitActionComponent* AsActionComponent = Cast<UUnitActionComponent>(From))
	{
		FActionCursorFinder BaseFinder = CreateCursorFinder(AsActionComponent);

		if (FActiveNodeMap* CurrentCursor = GetNodeMap(AsActionComponent))
		{
			CurrentCursor->BroadcastMessage_Montage(BaseFinder, Montage, bIsStart, bIsInterrupted);
		}
	}
}

TWeakObjectPtr<UActionExecutor> UActionExecutor::CreateExecutor(AActionBase* TargetAction, AOperator* TargetOperator, TArray<UUnitActionComponent*> TargetComponents, UActionNode* StartNode, const FExecutorValueMap& DefaultValues)
{
	if(!IsValid(TargetAction)) return nullptr;
	TargetComponents.RemoveAll([&](UUnitActionComponent* CurrentComponent)->bool{ return !IsValid(CurrentComponent);});
	if(TargetComponents.Num() == 0) return nullptr;
	TWeakObjectPtr<UActionExecutor> Result;
	UActionSetting::ClaimActivateExecutorFromPool(Result);
	if(!Result.IsValid()) return nullptr;
	Result->Action = TargetAction;
	Result->Operator = TargetOperator;
	Result->ValueMap = DefaultValues;
	for (UUnitActionComponent* CurrentComponent : TargetComponents)
	{
		if (!IsValid(CurrentComponent)) continue;
		Result->AddComponentToMap(CurrentComponent, StartNode);
	}
	return Result;

	return nullptr;
}

void UActionExecutor::DestroyExecutor(UActionExecutor* TargetExecutor)
{
	if (IsValid(TargetExecutor))	DestroyExecutorFromID(TargetExecutor->ExecutorID);
}

void UActionExecutor::DestroyExecutorFromID(int64 WantID)
{
	UActionSetting::ClaimDeactivateExecutorToPool(WantID);
}

UActionExecutor* UActionExecutor::GetExecutorFromID(int64 WantID)
{
	TWeakObjectPtr<UActionExecutor> Result = GetExecutorWeakPtrFromID(WantID);
	if (Result.IsValid()) return Result.Get();
	else return nullptr;
}

TWeakObjectPtr<UActionExecutor> UActionExecutor::GetExecutorWeakPtrFromID(int64 WantID)
{
	TWeakObjectPtr<UActionExecutor> Result;
	UActionSetting::ClaimGetExecutor(WantID, Result);
	return Result;
}
UActionExecutor* UActionExecutor::GetExecutorFromCursor(const FActionCursorFinder& WantCursor)
{
	return GetExecutorFromID(WantCursor.CurrentExecutorID);
}

void UActionExecutor::CompleteCursor(const FActionCursorFinder& WantCursor)
{
	TWeakObjectPtr<UActionExecutor> Target = GetExecutorWeakPtrFromID(WantCursor.CurrentExecutorID);
	if (Target.IsValid()) Target->CompleteNode(WantCursor);
}

void UActionExecutor::CancelCursor(const FActionCursorFinder& WantCursor)
{
	TWeakObjectPtr<UActionExecutor> Target = GetExecutorWeakPtrFromID(WantCursor.CurrentExecutorID);
	if (Target.IsValid()) Target->CancelNode(WantCursor);
}

UActionNode* UActionExecutor::GetNodeFromCursor(const FActionCursorFinder& WantCursor)
{
	return WantCursor.GetNode();
}