// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/Executables/ActionExecutor.h"
#include "Actions/Executables/ActionSelectorNode.h"
#include "Actions/ActionBase.h"
#include "Objects/Selectables/Components/UnitActionComponent.h"
#include "Objects/Selectables/Components/UnitMainComponent.h"
#include "Objects/Players/Operator.h"
#include "Interfaces/ActionSpawnable.h"
#include "Settings/ActionSetting.h"

const FExecutorValueMap FExecutorValueMap::Default;
FExecutorValueMap FExecutorValueMap::GarbageValueMap;


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



void FActiveNodeMap::Clear()
{
	NodeMap.Reset();
	ValueMap.Reset();
	EndEventMap.Reset();
}

int FActiveNodeMap::AddNode(UActionNode* Node)
{
	while (NodeMap.Find(nextID++));
	int Result = nextID - 1;
	SetNode<FActiveNodeInfo>(Node, Result);
	return Result;
}

int FActiveNodeMap::AddNode(UActionNode* Node, const FHitResult& WantHit)
{
	while (NodeMap.Find(nextID++));
	int Result = nextID - 1;
	FActiveNodeInfo_Hit& Info = SetNode<FActiveNodeInfo_Hit>(Node, Result);
	Info.SetHit(WantHit);
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
	if (const FActiveNodeInfo* CurrentInfo = GetInfo<FActiveNodeInfo>(ID)) return CurrentInfo->CurrentNode;
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

FExecutorValueMap* FActiveNodeMap::GetValueMap(int ID)
{
	FExecutorValueMap* Result = ValueMap.Find(ID);
	return Result;
}

void FActiveNodeMap::RemoveID(int ID)
{
	NodeMap.Remove(ID);
	EndEventMap.Remove(ID);
	ValueMap.Remove(ID);
}

void FActiveNodeMap::RemoveSubNodes()
{
	TInstancedStruct<FActiveNodeInfo>* MainNode = NodeMap.Find(0);
	NodeMap.Reset();
	if (MainNode)
	{
		NodeMap.Add(0, *MainNode);
	}
}

void FActiveNodeMap::BroadcastFunction(const FActionCursorFinder& Cursor, TFunctionRef<void(UActionNode*, const FActionCursorFinder&)> Function)
{
	if (IsEmpty()) return;
	TMap<int, TInstancedStruct<FActiveNodeInfo>> ReceivedNodeMap = NodeMap;

	for (const auto& CurrentPair : ReceivedNodeMap)
	{
		const FActiveNodeInfo& CurrentInfo = CurrentPair.Value.Get();
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

FExecutorValueMap& UActionExecutor::GetValueMap(const FActionCursorFinder& WantCursor, bool& bIsValid)
{
	try
	{
		FActiveNodeMap* NodeMap = GetNodeMap(WantCursor.CurrentComponent);
		if (NodeMap == nullptr) throw false;
		FExecutorValueMap* ValueMap = NodeMap->GetValueMap(WantCursor.CurrentID);
		bIsValid = ValueMap != nullptr;
		if (!bIsValid) throw false;

		return *ValueMap;
	}
	catch(...)
	{
		bIsValid = false;
		return FExecutorValueMap::GarbageValueMap;
	}
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
		FActiveNodeInfo* OriginInfo = CurrentNodeMap->GetInfo<FActiveNodeInfo>(ID);
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
	if (CurrentNodeMap) CurrentNodeInfo = &CurrentNodeMap->SetNode<FActiveNodeInfo>(TargetNode, ID);
	else CurrentNodeInfo = CursorMap.Add(TargetComponent, TargetNode).GetMainInfo();

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
		if (CurrentNodeMap) CurrentNodeInfo = CurrentNodeMap->GetInfo<FActiveNodeInfo>(ID);
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
	FExecutorValueMap* OriginValueMap = TargetInfo.GetValueMap(BaseCursor.CurrentID);
	if (OriginValueMap) TargetInfo.ValueMap.Add(ResultID, *OriginValueMap);
	EnterNode(NewCursor, TargetNode, true);
	return Result;
}

UActionNode* UActionExecutor::CreateSubNode(FActionCursorFinder BaseCursor, UActionNode* OriginNode, UActionNode* TargetNode, int& ResultID)
{
	FActiveNodeMap* CurrentNodeMap = GetOrAddNodeMap(BaseCursor.CurrentComponent);
	return CreateSubNode(BaseCursor, *CurrentNodeMap, OriginNode, TargetNode, ResultID);
}

UActionNode* UActionExecutor::CreateSubNode_Hit(FActionCursorFinder BaseCursor, UActionNode* OriginNode, UActionNode* TargetNode, const FHitResult& Hit, int& ResultID)
{
	FActiveNodeMap* CurrentNodeMap = GetOrAddNodeMap(BaseCursor.CurrentComponent);
	return CreateSubNode_Hit(BaseCursor, *CurrentNodeMap, OriginNode, TargetNode, Hit, ResultID);
}


UActionNode* UActionExecutor::CreateSubNodeWithEvent(FActionCursorFinder BaseCursor, UActionNode* OriginNode, UActionNode* TargetNode, int& ResultID, const FOnNodeEnded& OnNodeEnded)
{
	FActiveNodeMap* CurrentNodeMap = GetOrAddNodeMap(BaseCursor.CurrentComponent);
	return CreateSubNodeWithEvent(BaseCursor, *CurrentNodeMap, OriginNode, TargetNode, ResultID, OnNodeEnded);
}

UActionNode* UActionExecutor::CreateSubNode(FActionCursorFinder BaseCursor, FActiveNodeMap& TargetInfo, UActionNode* OriginNode, UActionNode* TargetNode, int& ResultID)
{
	ResultID = TargetInfo.AddNode(OriginNode);
	return InitiateSubNode(BaseCursor, TargetInfo, TargetNode, ResultID);
}

UActionNode* UActionExecutor::CreateSubNode_Hit(FActionCursorFinder BaseCursor, FActiveNodeMap& TargetInfo, UActionNode* OriginNode, UActionNode* TargetNode, const FHitResult& Hit, int& ResultID)
{
	ResultID = TargetInfo.AddNode(OriginNode, Hit);
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
		TArray<TInstancedStruct<FActiveNodeInfo>> DestroyTarget;
		for (const auto& CurrentNode : Cursor.NodeMap)
		{
			int CurrentID = CurrentNode.Key;
			if (CurrentID == 0 || CurrentID == exceptID) continue;
			SubCursor.CurrentID = CurrentID;
			Cursor.InvokeEndEvent(CurrentID, bIsCanceled);
			DestroyTarget.Add(CurrentNode.Value);
		}
		for (const TInstancedStruct<FActiveNodeInfo>& CurrentTarget : DestroyTarget)
		{
			const FActiveNodeInfo& TargetReference = CurrentTarget.Get();
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

void UActionExecutor::AddComponentToMap(UUnitActionComponent* TargetComponent, UActionNode* StartNode, const FExecutorValueMap& DefaultValues)
{
	if (!IsValid(TargetComponent)) return;
	TargetComponent->OnComponentRemoved.AddUniqueDynamic(this, &UActionExecutor::RemoveComponentBaseFromMap);
	TargetComponent->OnComponentMessage_Simple.AddUniqueDynamic(this, &UActionExecutor::OnMessageFromComponent_Simple);
	TargetComponent->OnComponentMessage_Detail.AddUniqueDynamic(this, &UActionExecutor::OnMessageFromComponent_Detail);
	TargetComponent->OnComponentMessage_Montage.AddUniqueDynamic(this, &UActionExecutor::OnMessageFromComponent_Montage);
	FActiveNodeMap& Result = CursorMap.Add(TargetComponent, StartNode);
	Result.ValueMap.Add(0, DefaultValues);
}

void UActionExecutor::AddComponentBaseToMap(UUnitComponentBase* TargetComponent, UActionNode* StartNode, const FExecutorValueMap& DefaultValues) { AddComponentToMap(Cast<UUnitActionComponent>(TargetComponent), StartNode, DefaultValues); }

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
	if (CursorMap.IsEmpty()) return nullptr;
	else if (FActiveNodeMap* ComponentInfo = CursorMap.Find(TargetComponent)) { return ComponentInfo; }
	else return nullptr;
}

FActiveNodeMap* UActionExecutor::AddNodeMap(UUnitActionComponent* TargetComponent)
{
	return &CursorMap.Add(TargetComponent);
}

FActiveNodeMap* UActionExecutor::GetOrAddNodeMap(UUnitActionComponent* TargetComponent)
{
	FActiveNodeMap* Result;
	if (CursorMap.IsEmpty()) Result = nullptr;
	else Result = CursorMap.Find(TargetComponent);
	if (Result == nullptr) Result = &CursorMap.Add(TargetComponent);
	return Result;
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
	for (UUnitActionComponent* CurrentComponent : TargetComponents)
	{
		if (!IsValid(CurrentComponent)) continue;
		Result->AddComponentToMap(CurrentComponent, StartNode, DefaultValues);
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

FExecutorValueMap& UActionExecutor::GetValueMapFromCursor(const FActionCursorFinder& WantCursor, bool& bIsValid)
{
	UActionExecutor* Executor = GetExecutorFromCursor(WantCursor);
	if (!IsValid(Executor))
	{
		bIsValid = false;
		return FExecutorValueMap::GarbageValueMap;
	}
	return Executor->GetValueMap(WantCursor, bIsValid);
}





bool UActionExecutor::HasFloat(const FActionCursorFinder& WantCursor, FName WantTag)
{
	bool bIsValidValueMap;
	FExecutorValueMap& ValueMap = GetValueMapFromCursor(WantCursor, bIsValidValueMap);
	if (!bIsValidValueMap) return false;
	return ValueMap.HasFloat(WantTag);
}

void UActionExecutor::SetFloat(const FActionCursorFinder& WantCursor, FName WantTag, const float& WantFloat)
{
	bool bIsValidValueMap;
	FExecutorValueMap& ValueMap = GetValueMapFromCursor(WantCursor, bIsValidValueMap);
	if (!bIsValidValueMap) return;
	ValueMap.SetFloat(WantTag, WantFloat);
}

float UActionExecutor::GetSavedFloat(const FActionCursorFinder& WantCursor, FName WantTag)
{
	bool bIsValidValueMap;
	FExecutorValueMap& ValueMap = GetValueMapFromCursor(WantCursor, bIsValidValueMap);
	if (!bIsValidValueMap) return 0.0f;
	return ValueMap.GetSavedFloat(WantCursor, WantTag);
}

bool UActionExecutor::HasPosition(const FActionCursorFinder& WantCursor, FName WantTag)
{
	bool bIsValidValueMap;
	FExecutorValueMap& ValueMap = GetValueMapFromCursor(WantCursor, bIsValidValueMap);
	if (!bIsValidValueMap) return false;
	return ValueMap.HasPosition(WantTag);
}

void UActionExecutor::SetPosition(const FActionCursorFinder& WantCursor, FName WantTag, const FVector& WantPosition)
{
	bool bIsValidValueMap;
	FExecutorValueMap& ValueMap = GetValueMapFromCursor(WantCursor, bIsValidValueMap);
	if (!bIsValidValueMap) return;
	return ValueMap.SetPosition(WantTag, WantPosition);
}

FVector UActionExecutor::GetSavedPosition(const FActionCursorFinder& WantCursor, FName WantTag)
{
	bool bIsValidValueMap;
	FExecutorValueMap& ValueMap = GetValueMapFromCursor(WantCursor, bIsValidValueMap);
	if (!bIsValidValueMap) return FVector::ZeroVector;
	return ValueMap.GetSavedPosition(WantCursor, WantTag);
}

bool UActionExecutor::HasDirection(const FActionCursorFinder& WantCursor, FName WantTag)
{
	bool bIsValidValueMap;
	FExecutorValueMap& ValueMap = GetValueMapFromCursor(WantCursor, bIsValidValueMap);
	if (!bIsValidValueMap) return false;
	return ValueMap.HasDirection(WantCursor, WantTag);
}

void UActionExecutor::SetDirection(FName WantTag, const FActionCursorFinder& WantCursor, const FVector& WantDirection)
{
	bool bIsValidValueMap;
	FExecutorValueMap& ValueMap = GetValueMapFromCursor(WantCursor, bIsValidValueMap);
	if (!bIsValidValueMap) return;
	return ValueMap.SetDirection(WantTag,WantCursor,WantDirection);
}

FVector UActionExecutor::GetSavedDirection(const FActionCursorFinder& WantCursor, FName WantTag)
{
	bool bIsValidValueMap;
	FExecutorValueMap& ValueMap = GetValueMapFromCursor(WantCursor, bIsValidValueMap);
	if (!bIsValidValueMap) return FVector::ZeroVector;
	return ValueMap.GetSavedDirection(WantCursor,WantTag);
}

void UActionExecutor::AddActor(const FActionCursorFinder& WantCursor, FName WantTag, AActor* WantActor)
{
	bool bIsValidValueMap;
	FExecutorValueMap& ValueMap = GetValueMapFromCursor(WantCursor, bIsValidValueMap);
	if (!bIsValidValueMap) return;
	return ValueMap.AddActor(WantTag, WantActor);
}

void UActionExecutor::RemoveActor(const FActionCursorFinder& WantCursor, FName WantTag, AActor* WantActor)
{
	bool bIsValidValueMap;
	FExecutorValueMap& ValueMap = GetValueMapFromCursor(WantCursor, bIsValidValueMap);
	if (!bIsValidValueMap) return;
	return ValueMap.RemoveActor(WantTag, WantActor);
}

AActor* UActionExecutor::GetSavedActor(const FActionCursorFinder& WantCursor, FName WantTag)
{
	bool bIsValidValueMap;
	FExecutorValueMap& ValueMap = GetValueMapFromCursor(WantCursor, bIsValidValueMap);
	if (!bIsValidValueMap) return nullptr;
	return ValueMap.GetSavedActor(WantCursor, WantTag);
}

TArray<AActor*>	UActionExecutor::GetSavedActorArray(const FActionCursorFinder& WantCursor, FName WantTag)
{
	bool bIsValidValueMap;
	FExecutorValueMap& ValueMap = GetValueMapFromCursor(WantCursor, bIsValidValueMap);
	if (!bIsValidValueMap) return TArray<AActor*>();
	return ValueMap.GetSavedActorArray(WantCursor, WantTag);
}
