// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/Executables/ActionExecutor.h"
#include "Actions/Executables/ActionSelectorNode.h"
#include "Actions/ActionBase.h"
#include "Objects/Selectables/Components/UnitActionComponent.h"
#include "Objects/Selectables/Components/UnitMainComponent.h"
#include "Objects/Players/Operator.h"
#include "Settings/ActionSetting.h"


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

void FActiveNodeMap::RemoveSubNodes()
{
	FActiveNodeInfo* MainNode = NodeMap.Find(0);
	NodeMap.Empty();
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
	FActiveNodeInfo* Result = nullptr;
	FActiveNodeMap* CurrentNodeMap = GetCursor(TargetComponent);
	 
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
			if (!TargetComponent->TrySetMainAction(WantCursor, TargetNode->Settings))
			{
				EndNode(WantCursor, OriginNode, false);
				return;
			}
		}
	}

	if (CurrentNodeMap) Result = &CurrentNodeMap->SetNode(TargetNode, ID);
	else Result = CursorMap.Add(TargetComponent, TargetNode).GetMainInfo();

	if (IsValid(TargetNode)) TargetNode->ClaimExecute(WantCursor);
	if (Result) Result->TryListeningStart();
}

UActionNode* UActionExecutor::CreateSubNode(FActionCursorFinder BaseCursor, FActiveNodeMap& TargetInfo, UActionNode* OriginNode, UActionNode* TargetNode, int& ResultID)
{
	ResultID = TargetInfo.AddNode(OriginNode);
	UActionNode* Result = TargetInfo.GetNode(ResultID);
	BaseCursor.CurrentID = ResultID;
	BaseCursor.bAsSubNode = true;
	EnterNode(BaseCursor, TargetNode, true);
	return Result;
}

UActionNode* UActionExecutor::GetCurrentNode(const FActionCursorFinder& WantCursor)
{
	UUnitActionComponent* TargetComponent = WantCursor.CurrentComponent;
	if (!IsValid(TargetComponent)) return nullptr;
	if (FActiveNodeMap* CursorFinder = GetCursor(TargetComponent))
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
	if (FActiveNodeMap* CursorFinder = GetCursor(TargetComponent))
	{
		FActiveNodeMap& Cursor = *CursorFinder;
		if (bEndSubNode && !Cursor.NodeMap.IsEmpty())
		{
			FActionCursorFinder SubCursor = WantCursor;
			TArray<FActiveNodeInfo> DestroyTarget;
			for (const auto& CurrentNode : Cursor.NodeMap)
			{
				int CurrentID = CurrentNode.Key;
				if (CurrentID == 0 || CurrentID == ID) continue;
				SubCursor.CurrentID = CurrentID;
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

		Cursor.RemoveID(ID);

		if (Cursor.IsEmpty())
		{
			if (UUnitMainComponent* Unit = TargetComponent->GetOwnerUnit()) Unit->NotifyExecutorEnded(ExecutorID, TargetComponent);
			CursorMap.Remove(TargetComponent);
			CheckCursorMap();
		}
	}
}

void UActionExecutor::CompleteNode(const FActionCursorFinder& WantCursor)
{
	UActionNode* WantNode = GetNode(WantCursor);
	if (!IsValid(WantNode)) return;
	WantNode->OnComplete(WantCursor);
	WantNode->MoveExecutorToNext(WantCursor);
}

void UActionExecutor::CancelNode(const FActionCursorFinder& WantCursor)
{
	UActionNode* WantNode = GetNode(WantCursor);
	if (!IsValid(WantNode)) return;
	WantNode->OnCancel(WantCursor);
	WantNode->MoveExecutorToCancel(WantCursor);
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

FActiveNodeMap* UActionExecutor::GetCursor(UUnitActionComponent* TargetComponent)
{
	if (&CursorMap == nullptr || CursorMap.IsEmpty()) return nullptr;
	else if (FActiveNodeMap* ComponentInfo = CursorMap.Find(TargetComponent)) { return ComponentInfo; }
	else return nullptr;
}

UActionNode* UActionExecutor::GetNode(const FActionCursorFinder& WantCursor)
{
	if (!WantCursor.CheckValid())return nullptr;
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



void UActionExecutor::OnMessageFromComponent_Simple(UUnitComponentBase* From, const FName& Message)
{
	if (UUnitActionComponent* AsActionComponent = Cast<UUnitActionComponent>(From))
	{
		FActionCursorFinder BaseFinder = CreateCursorFinder(AsActionComponent);

		if (FActiveNodeMap* CurrentCursor = GetCursor(AsActionComponent))
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

		if (FActiveNodeMap* CurrentCursor = GetCursor(AsActionComponent))
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

		if (FActiveNodeMap* CurrentCursor = GetCursor(AsActionComponent))
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