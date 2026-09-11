// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Actions/Values/ActionValueClaimer.h"
#include "Generals/Structs/ActionValueContainer.h"
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

	UPROPERTY(BlueprintReadOnly, Category = "Message")
	int ValueID = -1;

	FActiveNodeInfo() { }

	FActiveNodeInfo(int CurrentValueID) { ValueID = CurrentValueID; }

	FActiveNodeInfo(UActionNode* WantNode, int CurrentValueID, ENodeListeningState WantListeningState = ENodeListeningState::Pending)
	{
		ValueID = CurrentValueID;
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

	UPROPERTY(BlueprintReadOnly, Category = "Message")
	int ValueID = -1;

	int nextID = 0;

	void Clear();


	inline UActionNode* GetNode(int ID);

	FActiveNodeInfo* GetInfo(int ID) { return NodeMap.Find(ID); }

	const FActiveNodeInfo* GetInfo(int ID) const { return NodeMap.Find(ID); }

	FActiveNodeInfo* GetInfo(const FActionCursorFinder& TargetCursor) { return GetInfo(TargetCursor.CurrentID); }
	const FActiveNodeInfo* GetInfo(const FActionCursorFinder& TargetCursor) const { return GetInfo(TargetCursor.CurrentID); }

	inline FActiveNodeInfo* GetMainInfo() { return GetInfo(0); }

	FActiveNodeInfo* SetNode(UActionNode* TargetNode, int ID);
	bool SetEndEvent(int ID, const FOnNodeEnded& OnNodeEnded);

	int GetValueID(const FActionCursorFinder& TargetCursor) const;

	FActiveNodeInfo& AddMainNode(UActionNode* Node, int CurrentValueID);
	FActiveNodeInfo& AddMainNode(UActionNode* Node, FOnNodeEnded OnNodeEnded, int CurrentValueID);
	FActiveNodeInfo& AddNode(UActionNode* Node, int CurrentValueID, int& OutNodeID);
	FActiveNodeInfo& AddNode(UActionNode* Node, FOnNodeEnded OnNodeEnded, int CurrentValueID, int& OutNodeID);

	void InvokeEndEvent(int ID, bool bIsCanceled);
	inline void RemoveID(int ID);
	inline void RemoveSubNodes();
	inline bool IsEmpty() const { return NodeMap.IsEmpty(); }

	void BroadcastFunction(const FActionCursorFinder& Cursor, TFunctionRef<void(UActionNode*, const FActionCursorFinder&)> Function);
	void BroadcastMessage_Simple(const FActionCursorFinder& Cursor, FName Message);
	void BroadcastMessage_Detail(const FActionCursorFinder& Cursor, FName Message, const FName& Context);
	void BroadcastMessage_Montage(const FActionCursorFinder& Cursor, UAnimMontage* Montage, bool bIsStart, bool bIsInterrupted);

	FActiveNodeMap() {  }
	FActiveNodeMap(int CurrentValueID) { ValueID = CurrentValueID; }
	FActiveNodeMap(UActionNode* Node, int CurrentValueID) { SetNode(Node, 0); ValueID = CurrentValueID; }
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

	UPROPERTY(BlueprintReadOnly, Category = "Action")
	FActionValueContainer ValueContainer;

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

	UFUNCTION(BlueprintCallable, Category = "Action")
	FActiveNodeInfo& CreateMainNode(UUnitActionComponent* ActionCompoenent, UActionNode* RootNode, FActionCursorFinder& ResultCursor);

	UFUNCTION(BlueprintCallable, Category = "Action")
	FActiveNodeInfo& CreateSubNode(FActionCursorFinder BaseCursor, UActionNode* OriginNode, UActionNode* TargetNode, int& ResultID, FActionCursorFinder& ResultCursor);

	FActiveNodeInfo& AddMainNode(FActiveNodeMap& TargetInfo, UActionNode* RootNode);
	FActiveNodeInfo& AddSubNode(FActionCursorFinder BaseCursor, FActiveNodeMap& TargetInfo, UActionNode* OriginNode, UActionNode* TargetNode, int& ResultID);


	UFUNCTION(BlueprintPure, Category = "Action")
	UActionNode* GetNode(const FActionCursorFinder& WantCursor);

	FActiveNodeMap* GetActiveNodeMap(const FActionCursorFinder& WantCursor);
	const FActiveNodeMap* GetActiveNodeMap(const FActionCursorFinder& WantCursor) const;

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
	void AddComponentToMap(UUnitActionComponent* TargetComponent, UActionNode* StartNode, FActionCursorFinder& OutMainCursor);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void AddComponentBaseToMap(UUnitComponentBase* TargetComponent, UActionNode* StartNode, FActionCursorFinder& OutMainCursor);

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
	FActionCursorFinder CreateCursorFinder(UUnitActionComponent* TargetComponent, int TargetID = 0);

	FActiveNodeMap* GetNodeMap(UUnitActionComponent* TargetComponent);
	const FActiveNodeMap* GetNodeMap(UUnitActionComponent* TargetComponent) const;
	FActiveNodeMap* AddNodeMap(UUnitActionComponent* TargetComponent);
	FActiveNodeMap* GetOrAddNodeMap(UUnitActionComponent* TargetComponent);

	FActiveNodeInfo* GetNodeInfo(const FActionCursorFinder& Cursor);

	bool SetEndEvent(UUnitActionComponent* TargetComponent, int ID, const FOnNodeEnded& OnNodeEnded);
	bool SetEndEvent(const FActionCursorFinder& Cursor, const FOnNodeEnded& OnNodeEnded) { return SetEndEvent(Cursor.CurrentComponent, Cursor.CurrentID, OnNodeEnded); }
	bool SetEndEventOnMain(UUnitActionComponent* TargetComponent, const FOnNodeEnded& OnNodeEnded) { return SetEndEvent(TargetComponent, 0, OnNodeEnded); }

	bool GetValid() { return ExecutorID != -1; }

public:
	UFUNCTION(BlueprintCallable, Category = "Action")
	void OnMessageFromComponent_Simple(UUnitComponentBase* From, const FName& Message);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void OnMessageFromComponent_Detail(UUnitComponentBase* From, const FName& Message, const FName& Context);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void OnMessageFromComponent_Montage(UUnitComponentBase* From, UAnimMontage* Montage, bool bIsStart, bool bIsInterrupted);


public:

	UFUNCTION(BlueprintCallable, Category = "Action")
	static void ExecuteCursor(const FActionCursorFinder& WantCursor);

	UFUNCTION(BlueprintCallable, Category = "Action")
	static void ExecuteCursorWithInput(const FActionCursorFinder& WantCursor, const FInputPackage& Input);


	static  TWeakObjectPtr<UActionExecutor> CreateExecutor(AActionBase* TargetAction, AOperator* TargetOperator, TArray<UUnitActionComponent*> TargetComponents, UActionNode* StartNode, FActionCursorFinder& OutMainCursor);

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

	UFUNCTION(BlueprintPure, Category = "Action")
	static int GetValueIDFromCursor(const FActionCursorFinder& WantCursor);

public:
	UFUNCTION(BlueprintPure, Category = "ValueContainer")
	int GetValueID(const FActionCursorFinder& WantCursor) const;

	UFUNCTION(BlueprintPure, Category = "ValueContainer")
	int GetOrAddValueID(const FActionCursorFinder& WantCursor);


	UFUNCTION(BlueprintPure, Category = "ValueContainer")
	bool GetVector(const FActionCursorFinder& Cursor, const FName& Tag, FVector& OutResult, const FVector& DefaultValue) const;

	UFUNCTION(BlueprintPure, Category = "ValueContainer")
	static bool GetVectorFromCursor(const FActionCursorFinder& Cursor, const FName& Tag, FVector& OutResult, const FVector& DefaultValue);

	UFUNCTION(BlueprintCallable, Category = "ValueContainer")
	void SetVector(const FActionCursorFinder& Cursor, const FName& Tag, const FVector& Value);

	UFUNCTION(BlueprintCallable, Category = "ValueContainer")
	static void SetVectorToCursor(const FActionCursorFinder& Cursor, const FName& Tag, const FVector& Value);



	UFUNCTION(BlueprintPure, Category = "ValueContainer")
	bool GetHitResult(const FActionCursorFinder& Cursor, const FName& Tag, FHitResult& OutResult, const FHitResult& DefaultValue) const;

	UFUNCTION(BlueprintPure, Category = "ValueContainer")
	static bool GetHitResultFromCursor(const FActionCursorFinder& Cursor, const FName& Tag, FHitResult& OutResult, const FHitResult& DefaultValue);

	UFUNCTION(BlueprintCallable, Category = "ValueContainer")
	void SetHitResult(const FActionCursorFinder& Cursor, const FName& Tag, const FHitResult& Value);

	UFUNCTION(BlueprintCallable, Category = "ValueContainer")
	static void SetHitResultToCursor(const FActionCursorFinder& Cursor, const FName& Tag, const FHitResult& Value);




	UFUNCTION(BlueprintPure, Category = "ValueContainer")
	bool GetActor(const FActionCursorFinder& Cursor, const FName& Tag, AActor*& OutResult, AActor* DefaultValue) const;

	UFUNCTION(BlueprintPure, Category = "ValueContainer")
	static bool GetActorFromCursor(const FActionCursorFinder& Cursor, const FName& Tag, AActor*& OutResult, AActor* DefaultValue);

	UFUNCTION(BlueprintCallable, Category = "ValueContainer")
	void SetActor(const FActionCursorFinder& Cursor, const FName& Tag, AActor* Value);

	UFUNCTION(BlueprintCallable, Category = "ValueContainer")
	static void SetActorToCursor(const FActionCursorFinder& Cursor, const FName& Tag, AActor* Value);




	UFUNCTION(BlueprintPure, Category = "ValueContainer")
	bool GetClass(const FActionCursorFinder& Cursor, const FName& Tag, UClass*& OutResult, UClass* DefaultValue) const;

	UFUNCTION(BlueprintPure, Category = "ValueContainer")
	static bool GetClassFromCursor(const FActionCursorFinder& Cursor, const FName& Tag, UClass*& OutResult, UClass* DefaultValue);

	UFUNCTION(BlueprintCallable, Category = "ValueContainer")
	void SetClass(const FActionCursorFinder& Cursor, const FName& Tag, UClass* Value);

	UFUNCTION(BlueprintCallable, Category = "ValueContainer")
	static void SetClassToCursor(const FActionCursorFinder& Cursor, const FName& Tag, UClass* Value);




	UFUNCTION(BlueprintPure, Category = "ValueContainer")
	bool GetSoftObjectPath(const FActionCursorFinder& Cursor, const FName& Tag, FSoftObjectPath& OutResult, const FSoftObjectPath& DefaultValue) const;

	UFUNCTION(BlueprintPure, Category = "ValueContainer")
	static bool GetSoftObjectPathFromCursor(const FActionCursorFinder& Cursor, const FName& Tag, FSoftObjectPath& OutResult, const FSoftObjectPath& DefaultValue);

	UFUNCTION(BlueprintCallable, Category = "ValueContainer")
	void SetSoftObjectPath(const FActionCursorFinder& Cursor, const FName& Tag, const FSoftObjectPath& Value);

	UFUNCTION(BlueprintCallable, Category = "ValueContainer")
	static void SetSoftObjectPathToCursor(const FActionCursorFinder& Cursor, const FName& Tag, const FSoftObjectPath& Value);



	UFUNCTION(BlueprintPure, Category = "ValueContainer")
	bool GetBoolean(const FActionCursorFinder& Cursor, const FName& Tag, bool& OutResult, bool DefaultValue = false) const;

	UFUNCTION(BlueprintPure, Category = "ValueContainer")
	static bool GetBooleanFromCursor(const FActionCursorFinder& Cursor, const FName& Tag, bool& OutResult, bool DefaultValue = false);

	UFUNCTION(BlueprintCallable, Category = "ValueContainer")
	void SetBoolean(const FActionCursorFinder& Cursor, const FName& Tag, bool Value);

	UFUNCTION(BlueprintCallable, Category = "ValueContainer")
	static void SetBooleanToCursor(const FActionCursorFinder& Cursor, const FName& Tag, bool Value);




	UFUNCTION(BlueprintPure, Category = "ValueContainer")
	bool GetFloat(const FActionCursorFinder& Cursor, const FName& Tag, float& OutResult, float DefaultValue = 0.0f) const;

	UFUNCTION(BlueprintPure, Category = "ValueContainer")
	static bool GetFloatFromCursor(const FActionCursorFinder& Cursor, const FName& Tag, float& OutResult, float DefaultValue = 0.0f);

	UFUNCTION(BlueprintCallable, Category = "ValueContainer")
	void SetFloat(const FActionCursorFinder& Cursor, const FName& Tag, float Value);

	UFUNCTION(BlueprintCallable, Category = "ValueContainer")
	static void SetFloatToCursor(const FActionCursorFinder& Cursor, const FName& Tag, float Value);



	UFUNCTION(BlueprintPure, Category = "ValueContainer")
	bool GetDouble(const FActionCursorFinder& Cursor, const FName& Tag, double& OutResult, double DefaultValue = 0.0) const;

	UFUNCTION(BlueprintPure, Category = "ValueContainer")
	static bool GetDoubleFromCursor(const FActionCursorFinder& Cursor, const FName& Tag, double& OutResult, double DefaultValue = 0.0);

	UFUNCTION(BlueprintCallable, Category = "ValueContainer")
	void SetDouble(const FActionCursorFinder& Cursor, const FName& Tag, double Value);

	UFUNCTION(BlueprintCallable, Category = "ValueContainer")
	static void SetDoubleToCursor(const FActionCursorFinder& Cursor, const FName& Tag, double Value);



	UFUNCTION(BlueprintPure, Category = "ValueContainer")
	bool GetInteger(const FActionCursorFinder& Cursor, const FName& Tag, int32& OutResult, int32 DefaultValue = 0) const;

	UFUNCTION(BlueprintPure, Category = "ValueContainer")
	static bool GetIntegerFromCursor(const FActionCursorFinder& Cursor, const FName& Tag, int32& OutResult, int32 DefaultValue = 0);

	UFUNCTION(BlueprintCallable, Category = "ValueContainer")
	void SetInteger(const FActionCursorFinder& Cursor, const FName& Tag, int32 Value);

	UFUNCTION(BlueprintCallable, Category = "ValueContainer")
	static void SetIntegerToCursor(const FActionCursorFinder& Cursor, const FName& Tag, int32 Value);



	UFUNCTION(BlueprintPure, Category = "ValueContainer")
	bool GetInteger64(const FActionCursorFinder& Cursor, const FName& Tag, int64& OutResult, int64 DefaultValue = 0) const;

	UFUNCTION(BlueprintPure, Category = "ValueContainer")
	static bool GetInteger64FromCursor(const FActionCursorFinder& Cursor, const FName& Tag, int64& OutResult, int64 DefaultValue = 0);

	UFUNCTION(BlueprintCallable, Category = "ValueContainer")
	void SetInteger64(const FActionCursorFinder& Cursor, const FName& Tag, int64 Value);

	UFUNCTION(BlueprintCallable, Category = "ValueContainer")
	static void SetInteger64ToCursor(const FActionCursorFinder& Cursor, const FName& Tag, int64 Value);




	UFUNCTION(BlueprintPure, Category = "ValueContainer")
	bool GetByte(const FActionCursorFinder& Cursor, const FName& Tag, uint8& OutResult, uint8 DefaultValue = 0) const;

	UFUNCTION(BlueprintPure, Category = "ValueContainer")
	static bool GetByteFromCursor(const FActionCursorFinder& Cursor, const FName& Tag, uint8& OutResult, uint8 DefaultValue = 0);

	UFUNCTION(BlueprintCallable, Category = "ValueContainer")
	void SetByte(const FActionCursorFinder& Cursor, const FName& Tag, uint8 Value);

	UFUNCTION(BlueprintCallable, Category = "ValueContainer")
	static void SetByteToCursor(const FActionCursorFinder& Cursor, const FName& Tag, uint8 Value);



	UFUNCTION(BlueprintPure, Category = "ValueContainer")
	bool GetName(const FActionCursorFinder& Cursor, const FName& Tag, FName& OutResult, const FName& DefaultValue = NAME_None) const;

	UFUNCTION(BlueprintPure, Category = "ValueContainer")
	static bool GetNameFromCursor(const FActionCursorFinder& Cursor, const FName& Tag, FName& OutResult, const FName& DefaultValue = NAME_None);

	UFUNCTION(BlueprintCallable, Category = "ValueContainer")
	void SetName(const FActionCursorFinder& Cursor, const FName& Tag, const FName& Value);

	UFUNCTION(BlueprintCallable, Category = "ValueContainer")
	static void SetNameToCursor(const FActionCursorFinder& Cursor, const FName& Tag, const FName& Value);




	UFUNCTION(BlueprintPure, Category = "ValueContainer")
	bool GetString(const FActionCursorFinder& Cursor, const FName& Tag, FString& OutResult, const FString& DefaultValue = TEXT("")) const;

	UFUNCTION(BlueprintPure, Category = "ValueContainer")
	static bool GetStringFromCursor(const FActionCursorFinder& Cursor, const FName& Tag, FString& OutResult, const FString& DefaultValue = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "ValueContainer")
	void SetString(const FActionCursorFinder& Cursor, const FName& Tag, const FString& Value);

	UFUNCTION(BlueprintCallable, Category = "ValueContainer")
	static void SetStringToCursor(const FActionCursorFinder& Cursor, const FName& Tag, const FString& Value);




	UFUNCTION(BlueprintPure, Category = "ValueContainer")
	bool GetText(const FActionCursorFinder& Cursor, const FName& Tag, FText& OutResult, const FText& DefaultValue = FText::GetEmpty()) const;

	UFUNCTION(BlueprintPure, Category = "ValueContainer")
	static bool GetTextFromCursor(const FActionCursorFinder& Cursor, const FName& Tag, FText& OutResult, const FText& DefaultValue = FText::GetEmpty());

	UFUNCTION(BlueprintCallable, Category = "ValueContainer")
	void SetText(const FActionCursorFinder& Cursor, const FName& Tag, const FText& Value);

	UFUNCTION(BlueprintCallable, Category = "ValueContainer")
	static void SetTextToCursor(const FActionCursorFinder& Cursor, const FName& Tag, const FText& Value);


};
