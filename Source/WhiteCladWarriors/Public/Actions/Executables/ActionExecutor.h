// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Actions/Values/ActionValueClaimer.h"
#include "Generals/Structs/ActionStructures.h"
#include "Interfaces/ActionSpawnable.h"
#include "StructUtils/InstancedStruct.h"
#include "ActionExecutor.generated.h"

class AOperator;
class AActionBase;
class UUnitComponentBase;
class UUnitActionComponent;
class UActionSelectorNode;
class UActionNode;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionMessage_Simple, UUnitMainComponent*, From);
/**
 * 
 */
UENUM(BlueprintType)
enum class ENodeListeningState : uint8
{
	Mute, Pending, Listening
};

USTRUCT(BlueprintType)
struct FActiveNodeInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Action")
	TObjectPtr<UActionNode> CurrentNode = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Message")
	ENodeListeningState CurrentListeningState = ENodeListeningState::Pending;

	FActiveNodeInfo() {}

	FActiveNodeInfo(UActionNode* WantNode, ENodeListeningState WantListeningState = ENodeListeningState::Pending)
	{
		SetNode(WantNode);
		SetListening(WantListeningState);
	}

	inline UActionNode* SetNode(UActionNode* WantNode) { return CurrentNode = WantNode; }
	inline ENodeListeningState SetListening(ENodeListeningState NewState) { return CurrentListeningState = NewState; }

	inline ENodeListeningState TryListeningMute() { if(CurrentListeningState != ENodeListeningState::Mute) SetListening(ENodeListeningState::Mute); return CurrentListeningState; }
	inline ENodeListeningState TryListeningPending() { if(CurrentListeningState == ENodeListeningState::Listening) SetListening(ENodeListeningState::Pending); return CurrentListeningState; }
	inline ENodeListeningState TryListeningStart() { if (CurrentListeningState != ENodeListeningState::Listening) SetListening(ENodeListeningState::Listening); return CurrentListeningState; }
};

USTRUCT(BlueprintType)
struct FActiveNodeMap
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Action")
	TMap<int, FActiveNodeInfo> NodeMap;

	UPROPERTY(BlueprintReadOnly, Category = "Action")
	TMap<int, FOnNodeEnded> EndEventMap;

	int nextID = 1;

	void Clear();


	inline UActionNode* GetNode(int ID);

	FActiveNodeInfo* GetInfo(int ID)
	{
		if (NodeMap.IsEmpty()) return nullptr;
		if (FActiveNodeInfo* Result = NodeMap.Find(ID)) return Result;
		else return nullptr;
	}
	inline FActiveNodeInfo* GetMainInfo() { return GetInfo(0); }

	FActiveNodeInfo& SetNode(UActionNode* TargetNode, int ID)
	{
		FActiveNodeInfo* Info = GetInfo(ID);
		if (Info == nullptr)
		{
			FActiveNodeInfo& Instance = NodeMap.FindOrAdd(ID);
			Info = &Instance;
		}
		Info->SetNode(TargetNode);
		return *Info;
	}

	FActiveNodeInfo& SetNode(UActionNode* TargetNode, int ID, const FActiveNodeInfo& OriginCursor)
	{
		FActiveNodeInfo* Info = GetInfo(ID);
		if (Info == nullptr)
		{
			FActiveNodeInfo& Instance = NodeMap.FindOrAdd(ID, OriginCursor);
			Info = &Instance;
		}
		Info->SetNode(TargetNode);
		return *Info;
	}

	int AddNode(UActionNode* Node);
	int AddNode(UActionNode* Node, FOnNodeEnded OnNodeEnded);

	FExecutorValueMap* GetValueMap(int ID);

	void InvokeEndEvent(int ID, bool bIsCanceled);
	inline void RemoveID(int ID);
	inline void RemoveSubNodes();
	inline bool IsEmpty() const { return NodeMap.IsEmpty(); }

	void BroadcastFunction(const FActionCursorFinder& Cursor, TFunctionRef<void(UActionNode*, const FActionCursorFinder&)> Function);
	void BroadcastMessage_Simple(const FActionCursorFinder& Cursor, FName Message);
	void BroadcastMessage_Detail(const FActionCursorFinder& Cursor, FName Message, const FName& Context);
	void BroadcastMessage_Montage(const FActionCursorFinder& Cursor, UAnimMontage* Montage, bool bIsStart, bool bIsInterrupted);

	FActiveNodeMap() { }
	FActiveNodeMap(UActionNode* Node) { SetNode(Node, 0); }
};


UCLASS(Blueprintable, BlueprintType)
class WHITECLADWARRIORS_API UActionExecutor : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Action", Meta = (ExposeOnSpawn = "true"))
	TObjectPtr<AActionBase> Action;
	UPROPERTY(BlueprintReadOnly, Category = "Action", Meta = (ExposeOnSpawn = "true"))
	TObjectPtr<AOperator> Operator;

	UPROPERTY(BlueprintReadOnly, Category = "Action")
	TMap<UUnitActionComponent*, FActiveNodeMap> CursorMap;

	UPROPERTY(BlueprintReadOnly, Category = "Action")
	TArray<AActor*> CreatedActors;

	uint64 ExecutorID;

public:
	void Clear();

	UFUNCTION(BlueprintCallable, Category = "Action")
	void SetActionMessage_Simple(const FActionCursorFinder& WantCursor, FName Message);

	UFUNCTION(BlueprintPure, Category = "Action")
	TArray<UUnitActionComponent*> GetComponentArray() const;

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool SetInput(const FActionCursorFinder& WantCursor, UActionSelectorNode* WantNode, const FInputPackage& WantInput);

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool SetInputArray(TArray<FActionCursorFinder> CursorArray, UActionSelectorNode* WantNode, const FInputPackage& WantInput);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void EnterNode(const FActionCursorFinder& WantCursor, UActionNode* TargetNode, bool bIsCanceled, int RecursiveDepth = 12);


	UActionNode* InitiateSubNode(FActionCursorFinder& BaseCursor, FActiveNodeMap& TargetInfo, UActionNode* TargetNode, int& ResultID);

	UFUNCTION(BlueprintCallable, Category = "Action")
	UActionNode* CreateSubNode(FActionCursorFinder BaseCursor, UActionNode* OriginNode, UActionNode* TargetNode, int& ResultID);

	UFUNCTION(BlueprintCallable, Category = "Action")
	UActionNode* CreateSubNodeWithEvent(FActionCursorFinder BaseCursor, UActionNode* OriginNode, UActionNode* TargetNode, int& ResultID, const FOnNodeEnded& OnNodeEnded);

	UActionNode* CreateSubNode(FActionCursorFinder BaseCursor, FActiveNodeMap& TargetInfo, UActionNode* OriginNode, UActionNode* TargetNode, int& ResultID);
	UActionNode* CreateSubNodeWithEvent(FActionCursorFinder BaseCursor,FActiveNodeMap& TargetInfo, UActionNode* OriginNode, UActionNode* TargetNode, int& ResultID, const FOnNodeEnded& OnNodeEnded);


	UFUNCTION(BlueprintPure, Category = "Action")
	UActionNode* GetNode(const FActionCursorFinder& WantCursor);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void EndNode(const FActionCursorFinder& WantCursor, UActionNode* OldNode, bool bIsCanceled, bool bEndSubNode);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void EndSubNode(const FActionCursorFinder& WantCursor, bool bIsCanceled, int exceptID = -1);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void CompleteNode(const FActionCursorFinder& WantCursor);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void CancelNode(const FActionCursorFinder& WantCursor);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void InterruptNode(const FActionCursorFinder& WantCursor, const FActionCursorFinder& InterruptCursor, UActionNode* InterruptNode);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void AddComponentToMap(UUnitActionComponent* TargetComponent, UActionNode* StartNode, const FExecutorValueMap& DefaultValues);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void AddComponentBaseToMap(UUnitComponentBase* TargetComponent, UActionNode* StartNode, const FExecutorValueMap& DefaultValues);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void RemoveComponentFromMap(UUnitActionComponent* TargetComponent);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void RemoveComponentBaseFromMap(UUnitComponentBase* TargetComponent);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void CheckCursorMap();

	UFUNCTION(BlueprintCallable, Category = "Action")
	void AddCreatedActor(AActor* NewActor, UActionSpawnNode* SpawnNode, const FActionCursorFinder& BaseCursor);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void RemoveCreatedActor(AActor* OldActor, const FActionCursorFinder& BaseCursor);

	UFUNCTION(BlueprintCallable, Category = "Action")
	FActionCursorFinder CreateCursorFinder(UUnitActionComponent* TargetComponent, int TargetID = 0, bool bAsSubNode = false);

	FActiveNodeMap* GetNodeMap(UUnitActionComponent* TargetComponent);
	FActiveNodeMap* AddNodeMap(UUnitActionComponent* TargetComponent);
	FActiveNodeMap* GetOrAddNodeMap(UUnitActionComponent* TargetComponent);

	FActiveNodeInfo* GetNodeInfo(const FActionCursorFinder& Cursor)
	{
		if (&CursorMap == nullptr || CursorMap.IsEmpty()) return nullptr;
		else if (FActiveNodeMap* ComponentInfo = CursorMap.Find(Cursor.CurrentComponent))
		{
			return ComponentInfo->GetInfo(Cursor.CurrentID);
		}
		else return nullptr;
	}

	bool SetEndEventOnMainCursor(UUnitActionComponent* TargetComponent, const FOnNodeEnded& OnNodeEnded);

	bool GetValid() { return ExecutorID != -1; }

public:
	UFUNCTION(BlueprintCallable, Category = "Action")
	void OnMessageFromComponent_Simple(UUnitComponentBase* From, const FName& Message);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void OnMessageFromComponent_Detail(UUnitComponentBase* From, const FName& Message, const FName& Context);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void OnMessageFromComponent_Montage(UUnitComponentBase* From, UAnimMontage* Montage, bool bIsStart, bool bIsInterrupted);


public:
	static  TWeakObjectPtr<UActionExecutor> CreateExecutor(AActionBase* TargetAction, AOperator* TargetOperator, TArray<UUnitActionComponent*> TargetComponents, UActionNode* StartNode, const FExecutorValueMap& DefaultValues);

	UFUNCTION(BlueprintCallable, Category = "Action")
	static void DestroyExecutor(UActionExecutor* TargetExecutor);


	UFUNCTION(BlueprintCallable, Category = "Action")
	static void DestroyExecutorFromID(int64 WantID);

	UFUNCTION(BlueprintCallable, Category = "Action")
	static void CompleteCursor(const FActionCursorFinder& WantCursor);

	UFUNCTION(BlueprintCallable, Category = "Action")
	static void CancelCursor(const FActionCursorFinder& WantCursor);



	UFUNCTION(BlueprintPure, Category = "Action")
	static UActionExecutor* GetExecutorFromID(int64 WantID);
	static TWeakObjectPtr<UActionExecutor> GetExecutorWeakPtrFromID(int64 WantID);

	UFUNCTION(BlueprintPure, Category = "Action")
	static UActionExecutor* GetExecutorFromCursor(const FActionCursorFinder& WantCursor);

	UFUNCTION(BlueprintPure, Category = "Action")
	static UActionNode* GetNodeFromCursor(const FActionCursorFinder& WantCursor);
};
