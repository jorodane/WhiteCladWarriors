// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/Executables/ActionExecutor.h"
#include "Actions/Executables/ActionSelectorNode.h"
#include "Actions/ActionBase.h"
#include "Objects/Selectables/Components/UnitActionComponent.h"
#include "Objects/Selectables/Components/UnitMainComponent.h"
#include "Objects/Players/Operator.h"
#include "Interfaces/ActionSpawnable.h"
#include "Settings/ActionSetting.h"


void FActiveNodeMap::Clear()
{
	NodeMap.Reset();
	EndEventMap.Reset();
	ValueID = -1;
	nextID = 1;
}

FActiveNodeInfo* FActiveNodeMap::SetNode(UActionNode* TargetNode, int ID)
{
	FActiveNodeInfo* Info = GetInfo(ID);
	if (Info == nullptr) return Info;
	Info->SetNode(TargetNode);
	return Info;
}

int FActiveNodeMap::GetValueID(const FActionCursorFinder& TargetCursor) const
{
	const FActiveNodeInfo* TargetInfo = GetInfo(TargetCursor);
	if (TargetInfo == nullptr) return -1;
	return TargetInfo->ValueID;
}

FActiveNodeInfo& FActiveNodeMap::AddNode(UActionNode* Node, int CurrentValueID, int& OutNodeID)
{
	while (NodeMap.Find(nextID++));
	OutNodeID = nextID - 1;
	ValueID = CurrentValueID;
	return NodeMap.Add(OutNodeID, FActiveNodeInfo(Node, ValueID));
}

FActiveNodeInfo& FActiveNodeMap::AddNode(UActionNode* Node, FOnNodeEnded OnNodeEnded, int CurrentValueID, int& OutNodeID)
{
	FActiveNodeInfo& Result = AddNode(Node, CurrentValueID, OutNodeID);
	if (OutNodeID >= 0) EndEventMap.Add(OutNodeID, OnNodeEnded);
	return Result;
}

UActionNode* FActiveNodeMap::GetNode(int ID)
{
	if (const FActiveNodeInfo* CurrentInfo = GetInfo(ID)) return CurrentInfo->CurrentNode;
	else return nullptr;
}

void FActiveNodeMap::InvokeEndEvent(int ID, bool bIsCanceled)
{
	FOnNodeEnded Result;
	if (EndEventMap.RemoveAndCopyValue(ID, Result))
	{
		Result.Execute(bIsCanceled);
	}
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
	if (MainNode) NodeMap.Add(0, *MainNode);
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




void UActionExecutor::Clear()
{
	Action = nullptr;
	Operator = nullptr;
	CursorMap.Reset();
	CreatedActors.Reset();
	ValueContainer.Clear();
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

void UActionExecutor::EnterNode(const FActionCursorFinder& WantCursor, UActionNode* TargetNode, bool bIsCanceled, int RecursiveDepth)
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
			EndNode(WantCursor, OriginNode, bIsCanceled, false);
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
		const bool bWasMainAction = IsValid(OriginNode) ? OriginNode->GetIsMainAction() : false;
		const bool bIsMainAction = IsValid(TargetNode) ? TargetNode->GetIsMainAction() : false;

		const bool bExitMainLine = !bIsMainAction && bWasMainAction;
		const bool bEnterMainLine = bIsMainAction && !bWasMainAction;

		if (bExitMainLine) TargetComponent->EndMainAction(ExecutorID, TargetComponent);
		if (bEnterMainLine)
		{
			if (!TargetComponent->TrySetMainAction(WantCursor, TargetNode))
			{
				EndNode(WantCursor, OriginNode, true, false);
				return;
			}
		}
	}

	FActiveNodeInfo* CurrentNodeInfo = nullptr;
	if (CurrentNodeMap)
	{
		CurrentNodeInfo = CurrentNodeMap->SetNode(TargetNode, ID);
		if (CurrentNodeInfo == nullptr)
		{
			int NewValueID = ValueContainer.Registration(CurrentNodeMap->ValueID);
			CurrentNodeInfo = &CurrentNodeMap->AddNode(TargetNode, NewValueID, ID);
		}
	}
	if (IsValid(TargetNode))
	{
		TargetNode->ClaimExecute(WantCursor);
	}
	else
	{
		EndNode(WantCursor, OriginNode, false, false);
		return;
	}

	if (GetValid())
	{
		if (CurrentNodeMap) CurrentNodeInfo = CurrentNodeMap->GetInfo(ID);
		if (CurrentNodeInfo) CurrentNodeInfo->TryListeningStart();
	}
}

UActionNode* UActionExecutor::InitiateSubNode(FActionCursorFinder& BaseCursor, FActiveNodeMap& TargetInfo, UActionNode* TargetNode, int& ResultID)
{
	UActionNode* Result = TargetInfo.GetNode(ResultID);
	if (!Result) return nullptr;
	FActionCursorFinder NewCursor = BaseCursor;
	NewCursor.CurrentID = ResultID;
	NewCursor.bAsSubNode = true;
	EnterNode(NewCursor, TargetNode, true);
	return Result;
}

UActionNode* UActionExecutor::CreateSubNode(FActionCursorFinder BaseCursor, UActionNode* OriginNode, UActionNode* TargetNode, int& ResultID)
{
	FActiveNodeMap* CurrentNodeMap = GetOrAddNodeMap(BaseCursor.CurrentComponent);
	return CreateSubNode(BaseCursor, *CurrentNodeMap, OriginNode, TargetNode, ResultID);
}

UActionNode* UActionExecutor::CreateSubNodeWithEvent(FActionCursorFinder BaseCursor, UActionNode* OriginNode, UActionNode* TargetNode, int& ResultID, const FOnNodeEnded& OnNodeEnded)
{
	FActiveNodeMap* CurrentNodeMap = GetOrAddNodeMap(BaseCursor.CurrentComponent);
	return CreateSubNodeWithEvent(BaseCursor, *CurrentNodeMap, OriginNode, TargetNode, ResultID, OnNodeEnded);
}

UActionNode* UActionExecutor::CreateSubNode(FActionCursorFinder BaseCursor, FActiveNodeMap& TargetInfo, UActionNode* OriginNode, UActionNode* TargetNode, int& ResultID)
{
	int ValueID = ValueContainer.Registration(TargetInfo.GetValueID(BaseCursor));
	TargetInfo.AddNode(OriginNode, ValueID, ResultID);
	return InitiateSubNode(BaseCursor, TargetInfo, TargetNode, ResultID);
}

UActionNode* UActionExecutor::CreateSubNodeWithEvent(FActionCursorFinder BaseCursor, FActiveNodeMap& TargetInfo, UActionNode* OriginNode, UActionNode* TargetNode, int& ResultID, const FOnNodeEnded& OnNodeEnded)
{
	int ValueID = ValueContainer.Registration(TargetInfo.GetValueID(BaseCursor));
	TargetInfo.AddNode(OriginNode, OnNodeEnded, ValueID, ResultID);
	return InitiateSubNode(BaseCursor, TargetInfo, TargetNode, ResultID);
}

UActionNode* UActionExecutor::GetNode(const FActionCursorFinder& WantCursor)
{
	FActiveNodeMap* NodeMap = GetActiveNodeMap(WantCursor);
	if (NodeMap != nullptr) return NodeMap->GetNode(WantCursor.CurrentID);
	return nullptr;
}

FActiveNodeMap* UActionExecutor::GetActiveNodeMap(const FActionCursorFinder& WantCursor)
{
	UUnitActionComponent* TargetComponent = WantCursor.CurrentComponent;
	if (!IsValid(TargetComponent)) return nullptr;
	if (FActiveNodeMap* CursorFinder = GetNodeMap(TargetComponent)) return CursorFinder;
	return nullptr;
}

const FActiveNodeMap* UActionExecutor::GetActiveNodeMap(const FActionCursorFinder& WantCursor) const
{
	UUnitActionComponent* TargetComponent = WantCursor.CurrentComponent;
	if (!IsValid(TargetComponent)) return nullptr;
	if (const FActiveNodeMap* CursorFinder = GetNodeMap(TargetComponent)) return CursorFinder;
	return nullptr;
}

void UActionExecutor::EndNode(const FActionCursorFinder& WantCursor, UActionNode* OldNode, bool bIsCanceled, bool bEndSubNode)
{
	UUnitActionComponent* TargetComponent = WantCursor.CurrentComponent;
	int ID = WantCursor.CurrentID;
	if (!IsValid(TargetComponent)) return;
	if (WantCursor.CheckIsMainNode() && IsValid(OldNode) && OldNode->GetIsMainAction()) TargetComponent->EndMainAction(ExecutorID, TargetComponent);
	FActiveNodeMap* CursorFinder = GetNodeMap(TargetComponent);

	if (CursorFinder)
	{
		FActiveNodeMap& Cursor = *CursorFinder;
		if (bEndSubNode) EndSubNode(WantCursor, bIsCanceled, ID);
		Cursor.InvokeEndEvent(ID, bIsCanceled);
		Cursor.RemoveID(ID);

		if (Cursor.IsEmpty())
		{
			if (UUnitMainComponent* Unit = TargetComponent->GetOwnerUnit()) Unit->NotifyExecutorEnded(ExecutorID, TargetComponent);
			CursorMap.Remove(TargetComponent);
			CheckCursorMap();
		}
	}
}

void UActionExecutor::EndSubNode(const FActionCursorFinder& WantCursor, bool bIsCanceled, int exceptID)
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
			Cursor.InvokeEndEvent(CurrentID, bIsCanceled);
			DestroyTarget.Add(CurrentNode.Value);
		}
		for (const FActiveNodeInfo& CurrentTarget : DestroyTarget)
		{
			const FActiveNodeInfo& TargetReference = CurrentTarget;
			if (IsValid(TargetReference.CurrentNode))
			{
				TargetReference.CurrentNode->MoveExecutorToCancel(SubCursor);
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
	AddNodeMap(TargetComponent);
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

void UActionExecutor::AddCreatedActor(AActor* NewActor, UActionSpawnNode* SpawnNode, const FActionCursorFinder& BaseCursor)
{
	if (!IsValid(NewActor)) return;
	CreatedActors.AddUnique(NewActor);
	if (NewActor->GetClass()->ImplementsInterface(UActionSpawnable::StaticClass()))
	{
		FActionCursorFinder NewCursor = BaseCursor;
		NewCursor.ClaimActor = NewActor;
		FOnSpawnedActorDestroyed ActorRemoveEvent;
		ActorRemoveEvent.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(UActionExecutor, RemoveCreatedActor));

		IActionSpawnable::Execute_SpawnInitialize(NewActor, SpawnNode, NewCursor, ActorRemoveEvent);
	}
}

void UActionExecutor::RemoveCreatedActor(AActor* OldActor, const FActionCursorFinder& BaseCursor)
{
	CreatedActors.Remove(OldActor);
	CheckCursorMap();
}


FActionCursorFinder UActionExecutor::CreateCursorFinder(UUnitActionComponent* TargetComponent, int TargetID, bool bAsSubNode)
{
	return FActionCursorFinder(Action, Operator, ExecutorID, TargetComponent, TargetID, bAsSubNode);
}

FActiveNodeMap* UActionExecutor::GetNodeMap(UUnitActionComponent* TargetComponent)
{
	return CursorMap.Find(TargetComponent);
}

const FActiveNodeMap* UActionExecutor::GetNodeMap(UUnitActionComponent* TargetComponent) const
{
	return CursorMap.Find(TargetComponent);
}

FActiveNodeMap* UActionExecutor::AddNodeMap(UUnitActionComponent* TargetComponent)
{
	int NewValueID = ValueContainer.Registration(TargetComponent);
	return &CursorMap.Add(TargetComponent, FActiveNodeMap(NewValueID));
}

FActiveNodeMap* UActionExecutor::GetOrAddNodeMap(UUnitActionComponent* TargetComponent)
{
	FActiveNodeMap* Result;
	if (CursorMap.IsEmpty()) Result = nullptr;
	else Result = CursorMap.Find(TargetComponent);
	if (Result == nullptr) Result = AddNodeMap(TargetComponent);
	return Result;
}


FActiveNodeInfo* UActionExecutor::GetNodeInfo(const FActionCursorFinder& Cursor)
{
	if (&CursorMap == nullptr || CursorMap.IsEmpty()) return nullptr;
	else if (FActiveNodeMap* ComponentInfo = CursorMap.Find(Cursor.CurrentComponent))
	{
		return ComponentInfo->GetInfo(Cursor.CurrentID);
	}
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

TWeakObjectPtr<UActionExecutor> UActionExecutor::CreateExecutor(AActionBase* TargetAction, AOperator* TargetOperator, TArray<UUnitActionComponent*> TargetComponents, UActionNode* StartNode)
{
	if(!IsValid(TargetAction)) return nullptr;
	TargetComponents.RemoveAll([&](UUnitActionComponent* CurrentComponent)->bool{ return !IsValid(CurrentComponent);});
	if(TargetComponents.Num() == 0) return nullptr;
	TWeakObjectPtr<UActionExecutor> Result;
	UActionSetting::ClaimActivateExecutorFromPool(Result);
	if(!Result.IsValid()) return nullptr;
	Result->Action = TargetAction;
	Result->Operator = TargetOperator;
	for (UUnitActionComponent* CurrentComponent : TargetComponents)
	{
		if (!IsValid(CurrentComponent)) continue;
		Result->AddComponentToMap(CurrentComponent, StartNode);
	}
	return Result;
}

void UActionExecutor::DestroyExecutor(UActionExecutor* TargetExecutor)
{
	if (IsValid(TargetExecutor))	DestroyExecutorFromID(TargetExecutor->ExecutorID);
}

void UActionExecutor::DestroyExecutorFromID(int64 WantID)
{
	UActionSetting::ClaimDeactivateExecutorToPool(WantID);
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

UActionNode* UActionExecutor::GetNodeFromCursor(const FActionCursorFinder& WantCursor)
{
	return WantCursor.GetNode();
}

int UActionExecutor::GetValueIDFromCursor(const FActionCursorFinder& WantCursor)
{
	UActionExecutor* Executor = GetExecutorFromCursor(WantCursor);
	if (!IsValid(Executor)) return FActionValueContainer::InvalidID;
	return Executor->GetValueID(WantCursor);
}

int UActionExecutor::GetValueID(const FActionCursorFinder& WantCursor) const
{
	const FActiveNodeMap* NodeMap = GetActiveNodeMap(WantCursor);
	if (NodeMap == nullptr) return FActionValueContainer::InvalidID;
	return NodeMap->GetValueID(WantCursor);
}

int UActionExecutor::GetOrAddValueID(const FActionCursorFinder& WantCursor)
{
	const FActiveNodeMap* NodeMap = GetActiveNodeMap(WantCursor);
	if (NodeMap == nullptr)
	{
		if(!IsValid(WantCursor.CurrentComponent)) return FActionValueContainer::InvalidID;
		return ValueContainer.Registration(WantCursor.CurrentComponent);
	}
	return NodeMap->GetValueID(WantCursor);
}

#define DEFINE_ACTION_VALUE_FUNCTIONS(TypeName, Type, DefaultParam, Getter, Setter) \
bool UActionExecutor::Get##TypeName(const FActionCursorFinder& Cursor, const FName& Tag, Type& OutResult, DefaultParam DefaultValue) const \
{ \
	const int ValueID = GetValueID(Cursor); \
	if (ValueID <= FActionValueContainer::InvalidID) \
	{ \
		OutResult = DefaultValue; \
		return false; \
	} \
	return ValueContainer.Getter(ValueID, Tag, OutResult, DefaultValue); \
} \
\
bool UActionExecutor::Get##TypeName##FromCursor( const FActionCursorFinder& Cursor, const FName& Tag, Type& OutResult, DefaultParam DefaultValue) \
{ \
	UActionExecutor* Executor = GetExecutorFromCursor(Cursor); \
	if (!IsValid(Executor)) \
	{ \
		OutResult = DefaultValue; \
		return false; \
	} \
	return Executor->Get##TypeName(Cursor, Tag, OutResult, DefaultValue); \
} \
\
void UActionExecutor::Set##TypeName(const FActionCursorFinder& Cursor, const FName& Tag, DefaultParam Value) \
{ \
	const int ValueID = GetOrAddValueID(Cursor); \
	if (ValueID == FActionValueContainer::InvalidID) return; \
	ValueContainer.Setter(ValueID, Tag, Value); \
} \
\
void UActionExecutor::Set##TypeName##ToCursor( const FActionCursorFinder& Cursor, const FName& Tag, DefaultParam Value) \
{ \
	UActionExecutor* Executor = GetExecutorFromCursor(Cursor); \
	if (IsValid(Executor)) \
	{ \
		Executor->Set##TypeName(Cursor, Tag, Value); \
	} \
}

#define DEFINE_ACTION_STRUCT_FUNCTIONS(STRUCT_NAME) DEFINE_ACTION_VALUE_FUNCTIONS(STRUCT_NAME, F##STRUCT_NAME, const F##STRUCT_NAME&, GetStruct<F##STRUCT_NAME>, SetStruct<F##STRUCT_NAME>)
#define DEFINE_ACTION_OBJECT_FUNCTIONS(OBJECT_NAME) DEFINE_ACTION_VALUE_FUNCTIONS(OBJECT_NAME, A##OBJECT_NAME*, A##OBJECT_NAME*, GetObject<A##OBJECT_NAME>, SetObject<A##OBJECT_NAME>)
#define DEFINE_ACTION_SENTENCE_FUNCTIONS(SENTENCE_NAME) DEFINE_ACTION_VALUE_FUNCTIONS(SENTENCE_NAME, F##SENTENCE_NAME, const F##SENTENCE_NAME&, Get##SENTENCE_NAME, Set##SENTENCE_NAME)
#define DEFINE_ACTION_SOFTPATH_FUNCTIONS(SOFTPATH_NAME) DEFINE_ACTION_VALUE_FUNCTIONS(SOFTPATH_NAME, F##SOFTPATH_NAME, const F##SOFTPATH_NAME&, Get##SOFTPATH_NAME, Set##SOFTPATH_NAME)
#define DEFINE_ACTION_CLASSTYPE_FUNCTIONS(CLASSTYPE_NAME) DEFINE_ACTION_VALUE_FUNCTIONS(CLASSTYPE_NAME, U##CLASSTYPE_NAME*, U##CLASSTYPE_NAME*, Get##CLASSTYPE_NAME, Set##CLASSTYPE_NAME)
#define DEFINE_ACTION_NUMERAL_FUNCTIONS(FUNCTION_NAME, TYPE_NAME) DEFINE_ACTION_VALUE_FUNCTIONS(FUNCTION_NAME, TYPE_NAME, TYPE_NAME, Get##FUNCTION_NAME, Set##FUNCTION_NAME)

//DEFINE_ACTION_STRUCT_FUNCTIONS(Vector)
bool UActionExecutor::GetVector(const FActionCursorFinder& Cursor, const FName& Tag, FVector& OutResult, const FVector& DefaultValue) const
{
	const int ValueID = GetValueID(Cursor);
	if (ValueID <= FActionValueContainer::InvalidID)
	{
		OutResult = DefaultValue;
		return false;
	}
	return ValueContainer.GetStruct(ValueID, Tag, OutResult, DefaultValue);
}

bool UActionExecutor::GetVectorFromCursor(const FActionCursorFinder& Cursor,const FName& Tag,FVector& OutResult,const FVector& DefaultValue)
{
	UActionExecutor* Executor = GetExecutorFromCursor(Cursor);
	if (!IsValid(Executor))
	{
		OutResult = DefaultValue;
		return false;
	}
	return Executor->GetVector(Cursor, Tag, OutResult, DefaultValue);
}

void UActionExecutor::SetVector(const FActionCursorFinder& Cursor,const FName& Tag,const FVector& Value)
{
	const int ValueID = GetValueID(Cursor);
	if (ValueID == FActionValueContainer::InvalidID) return;
	ValueContainer.SetStruct(ValueID, Tag, Value);
}

void UActionExecutor::SetVectorToCursor(
	const FActionCursorFinder& Cursor,
	const FName& Tag,
	const FVector& Value)
{
	UActionExecutor* Executor = GetExecutorFromCursor(Cursor);
	if (IsValid(Executor))
	{
		Executor->SetVector(Cursor, Tag, Value);
	}
}
DEFINE_ACTION_STRUCT_FUNCTIONS(HitResult)
DEFINE_ACTION_OBJECT_FUNCTIONS(Actor)
DEFINE_ACTION_CLASSTYPE_FUNCTIONS(Class)
DEFINE_ACTION_SOFTPATH_FUNCTIONS(SoftObjectPath)

DEFINE_ACTION_NUMERAL_FUNCTIONS(Boolean, bool)
DEFINE_ACTION_NUMERAL_FUNCTIONS(Float, float)
DEFINE_ACTION_NUMERAL_FUNCTIONS(Double, double)
DEFINE_ACTION_NUMERAL_FUNCTIONS(Integer, int32)
DEFINE_ACTION_NUMERAL_FUNCTIONS(Integer64, int64)
DEFINE_ACTION_NUMERAL_FUNCTIONS(Byte, uint8)

DEFINE_ACTION_SENTENCE_FUNCTIONS(Name)
DEFINE_ACTION_SENTENCE_FUNCTIONS(String)
DEFINE_ACTION_SENTENCE_FUNCTIONS(Text)







//bool UActionExecutor::HasFloat(const FActionCursorFinder& WantCursor, FName WantTag)
//{
//	bool bIsValidValueMap;
//	FExecutorValueMap& ValueMap = GetValueMapFromCursor(WantCursor, bIsValidValueMap);
//	if (!bIsValidValueMap) return false;
//	return ValueMap.HasFloat(WantTag);
//}
//
//void UActionExecutor::SetFloat(const FActionCursorFinder& WantCursor, FName WantTag, const float& WantFloat)
//{
//	bool bIsValidValueMap;
//	FExecutorValueMap& ValueMap = GetValueMapFromCursor(WantCursor, bIsValidValueMap);
//	if (!bIsValidValueMap) return;
//	ValueMap.SetFloat(WantTag, WantFloat);
//}
//
//float UActionExecutor::GetSavedFloat(const FActionCursorFinder& WantCursor, FName WantTag)
//{
//	bool bIsValidValueMap;
//	FExecutorValueMap& ValueMap = GetValueMapFromCursor(WantCursor, bIsValidValueMap);
//	if (!bIsValidValueMap) return 0.0f;
//	return ValueMap.GetSavedFloat(WantCursor, WantTag);
//}
//
//bool UActionExecutor::HasPosition(const FActionCursorFinder& WantCursor, FName WantTag)
//{
//	bool bIsValidValueMap;
//	FExecutorValueMap& ValueMap = GetValueMapFromCursor(WantCursor, bIsValidValueMap);
//	if (!bIsValidValueMap) return false;
//	return ValueMap.HasPosition(WantTag);
//}
//
//void UActionExecutor::SetPosition(const FActionCursorFinder& WantCursor, FName WantTag, const FVector& WantPosition)
//{
//	bool bIsValidValueMap;
//	FExecutorValueMap& ValueMap = GetValueMapFromCursor(WantCursor, bIsValidValueMap);
//	if (!bIsValidValueMap) return;
//	return ValueMap.SetPosition(WantTag, WantPosition);
//}
//
//FVector UActionExecutor::GetSavedPosition(const FActionCursorFinder& WantCursor, FName WantTag)
//{
//	bool bIsValidValueMap;
//	FExecutorValueMap& ValueMap = GetValueMapFromCursor(WantCursor, bIsValidValueMap);
//	if (!bIsValidValueMap) return FVector::ZeroVector;
//	return ValueMap.GetSavedPosition(WantCursor, WantTag);
//}
//
//bool UActionExecutor::HasDirection(const FActionCursorFinder& WantCursor, FName WantTag)
//{
//	bool bIsValidValueMap;
//	FExecutorValueMap& ValueMap = GetValueMapFromCursor(WantCursor, bIsValidValueMap);
//	if (!bIsValidValueMap) return false;
//	return ValueMap.HasDirection(WantCursor, WantTag);
//}
//
//void UActionExecutor::SetDirection(FName WantTag, const FActionCursorFinder& WantCursor, const FVector& WantDirection)
//{
//	bool bIsValidValueMap;
//	FExecutorValueMap& ValueMap = GetValueMapFromCursor(WantCursor, bIsValidValueMap);
//	if (!bIsValidValueMap) return;
//	return ValueMap.SetDirection(WantTag,WantCursor,WantDirection);
//}
//
//FVector UActionExecutor::GetSavedDirection(const FActionCursorFinder& WantCursor, FName WantTag)
//{
//	bool bIsValidValueMap;
//	FExecutorValueMap& ValueMap = GetValueMapFromCursor(WantCursor, bIsValidValueMap);
//	if (!bIsValidValueMap) return FVector::ZeroVector;
//	return ValueMap.GetSavedDirection(WantCursor,WantTag);
//}
//
//void UActionExecutor::AddActor(const FActionCursorFinder& WantCursor, FName WantTag, AActor* WantActor)
//{
//	bool bIsValidValueMap;
//	FExecutorValueMap& ValueMap = GetValueMapFromCursor(WantCursor, bIsValidValueMap);
//	if (!bIsValidValueMap) return;
//	return ValueMap.AddActor(WantTag, WantActor);
//}
//
//void UActionExecutor::RemoveActor(const FActionCursorFinder& WantCursor, FName WantTag, AActor* WantActor)
//{
//	bool bIsValidValueMap;
//	FExecutorValueMap& ValueMap = GetValueMapFromCursor(WantCursor, bIsValidValueMap);
//	if (!bIsValidValueMap) return;
//	return ValueMap.RemoveActor(WantTag, WantActor);
//}
//
//AActor* UActionExecutor::GetSavedActor(const FActionCursorFinder& WantCursor, FName WantTag)
//{
//	bool bIsValidValueMap;
//	FExecutorValueMap& ValueMap = GetValueMapFromCursor(WantCursor, bIsValidValueMap);
//	if (!bIsValidValueMap) return nullptr;
//	return ValueMap.GetSavedActor(WantCursor, WantTag);
//}
//
//TArray<AActor*>	UActionExecutor::GetSavedActorArray(const FActionCursorFinder& WantCursor, FName WantTag)
//{
//	bool bIsValidValueMap;
//	FExecutorValueMap& ValueMap = GetValueMapFromCursor(WantCursor, bIsValidValueMap);
//	if (!bIsValidValueMap) return TArray<AActor*>();
//	return ValueMap.GetSavedActorArray(WantCursor, WantTag);
//}
