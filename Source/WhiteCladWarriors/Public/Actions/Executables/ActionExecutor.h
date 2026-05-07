// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Actions/Values/ActionValueClaimer.h"
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
		CurrentNode = WantNode;
		SetListening(WantListeningState);
	}

	inline ENodeListeningState SetListening(ENodeListeningState NewState) { return CurrentListeningState = NewState; }

	inline ENodeListeningState TryListeningPending() { if(CurrentListeningState == ENodeListeningState::Listening) SetListening(ENodeListeningState::Pending); return CurrentListeningState; }
	inline ENodeListeningState TryListeningStart() { if (CurrentListeningState == ENodeListeningState::Pending) SetListening(ENodeListeningState::Listening); return CurrentListeningState; }
};

USTRUCT(BlueprintType)
struct FActiveNodeMap
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Action")
	TMap<int, FActiveNodeInfo> NodeMap;

	int nextID = 1;

	int AddNode(UActionNode* Node);
	inline UActionNode* GetNode(int ID);
	inline FActiveNodeInfo* GetInfo(int ID);
	inline FActiveNodeInfo& SetNode(UActionNode* Node, int ID);

	inline void RemoveID(int ID);
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

	TMap<FName, FVector> PositionMap;
	TMap<TPair<UUnitActionComponent*, FName>, FVector> DirectionMap;
	TMultiMap<FName, AActor*> ActorMultiMap;


public:
	UFUNCTION(BlueprintCallable, Category = "Action")
	void SetActionMessage_Simple(const FActionCursorFinder& WantCursor, FName Message);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void SetPosition(FName WantTag, const FVector& WantPosition);

	UFUNCTION(BlueprintPure, Category = "Action")
	FVector GetSavedPosition(const FActionCursorFinder& WantCursor, FName WantTag) const;

	UFUNCTION(BlueprintPure, Category = "Action")
	TArray<UUnitActionComponent*> GetComponentArray() const;

	UFUNCTION(BlueprintPure, Category = "Action")
	bool HasPosition(FName WantTag) const;

	UFUNCTION(BlueprintCallable, Category = "Action")
	void SetDirection(FName WantTag, const FActionCursorFinder& WantCursor, const FVector& WantDirection);

	UFUNCTION(BlueprintPure, Category = "Action")
	FVector GetSavedDirection(const FActionCursorFinder& WantCursor, FName WantTag) const;

	UFUNCTION(BlueprintPure, Category = "Action")
	bool HasDirection(const FActionCursorFinder& WantCursor, FName WantTag) const;

	UFUNCTION(BlueprintCallable, Category = "Action")
	void AddActor(FName WantTag, AActor* WantActor);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void RemoveActor(FName WantTag, AActor* WantActor);

	UFUNCTION(BlueprintPure, Category = "Action")
	AActor* GetSavedActor(const FActionCursorFinder& WantCursor, FName WantTag) const;

	UFUNCTION(BlueprintPure, Category = "Action")
	TArray<AActor*> GetSavedActorArray(const FActionCursorFinder& WantCursor, FName WantTag) const;

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool SetInput(const FActionCursorFinder& WantCursor, UActionSelectorNode* WantNode, const FInputPackage& WantInput);

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool SetInputArray(TArray<FActionCursorFinder> CursorArray, UActionSelectorNode* WantNode, const FInputPackage& WantInput);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void EnterNode(const FActionCursorFinder& WantCursor, UActionNode* TargetNode, bool bIsSubNode = false, int RecursiveDepth = 12);

	UFUNCTION(BlueprintCallable, Category = "Action")
	UActionNode* CreateSubNode(FActionCursorFinder BaseCursor, FActiveNodeMap& TargetInfo, UActionNode* OriginNode, UActionNode* TargetNode, int& ResultID);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void EndNode(const FActionCursorFinder& WantCursor, UActionNode* OldNode);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void CompleteNode(const FActionCursorFinder& WantCursor);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void CancelNode(const FActionCursorFinder& WantCursor);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void AddComponentToMap(UUnitActionComponent* TargetComponent, UActionNode* StartNode);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void AddComponentBaseToMap(UUnitComponentBase* TargetComponent, UActionNode* StartNode);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void RemoveComponentFromMap(UUnitActionComponent* TargetComponent);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void RemoveComponentBaseFromMap(UUnitComponentBase* TargetComponent);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void CheckCursorMap();

	UFUNCTION(BlueprintCallable, Category = "Action")
	FActionCursorFinder CreateCursorFinder(UUnitActionComponent* TargetComponent, int TargetID = 0);

	FActiveNodeMap* GetCursor(UUnitActionComponent* TargetComponent);
	UActionNode* GetNode(const FActionCursorFinder& WantCursor);
	UActionNode* GetNode(UUnitActionComponent* TargetComponent, int TargetID = 0);

public:
	UFUNCTION(BlueprintCallable, Category = "Action")
	void OnMessageFromComponent_Simple(UUnitComponentBase* From, const FName& Message);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void OnMessageFromComponent_Detail(UUnitComponentBase* From, const FName& Message, const FName& Context);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void OnMessageFromComponent_Montage(UUnitComponentBase* From, UAnimMontage* Montage, bool bIsStart, bool bIsInterrupted);


public:
	UFUNCTION(BlueprintCallable, Category = "Action")
	static UActionExecutor* CreateExecutor(AActionBase* TargetAction, AOperator* TargetOperator, TArray<UUnitActionComponent*> TargetComponents, UActionNode* StartNode);

	UFUNCTION(BlueprintCallable, Category = "Action")
	static void DestroyExecutor(UActionExecutor* TargetExecutor);

	UFUNCTION(BlueprintCallable, Category = "Action")
	static void CompleteCursor(const FActionCursorFinder& WantCursor);

	UFUNCTION(BlueprintCallable, Category = "Action")
	static void CancelCursor(const FActionCursorFinder& WantCursor);
};
