// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Generals/Structs/InputPackage.h"
#include "Generals/Structs/ActionStructures.h"
#include "ActionNode.generated.h"

class UActionExecutor;
class UUnitActionComponent;
/**
 * 
 */

USTRUCT(BlueprintType)
struct FLinkedNodeInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	TObjectPtr<UActionNode> Node;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	bool bIsSubNode = true;
};

UCLASS( Blueprintable, BlueprintType )
class WHITECLADWARRIORS_API UActionNode : public UObject
{
	GENERATED_BODY()

public:
	static const FActionExecuteSettingContainer DefaultExecuteSetting;

public:
	UPROPERTY(BlueprintReadWrite, Category = "Action", Meta = (ExposeOnSpawn = "true"))
	TObjectPtr<UActionNode> NextNode;

	UPROPERTY(BlueprintReadWrite, Category = "Action", Meta = (ExposeOnSpawn = "true"))
	TObjectPtr<UActionNode> CanceledNode;

	UPROPERTY(BlueprintReadWrite, Category = "Action", Meta = (ExposeOnSpawn = "true"))
	TObjectPtr<UActionNode> BlockedNode;

	UPROPERTY(BlueprintReadWrite, Category = "Action", Meta = (ExposeOnSpawn = "true"))
	TMap<FName, FLinkedNodeInfo> LinkedNodes;
	
public:
	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Indicator")
	float GetPercent(const FActionCursorFinder& Cursor);
	float GetPercent_Implementation(const FActionCursorFinder& Cursor) { return 0.0f; }

	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Action")
	bool GetIsMainAction();
	virtual bool GetIsMainAction_Implementation() { return false; };

	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Action")
	const FActionExecuteSettingContainer GetExecuteSetting();
	virtual const FActionExecuteSettingContainer GetExecuteSetting_Implementation() { return DefaultExecuteSetting; };

	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Action")
	bool GetCanEnter(const FActionCursorFinder& WantCursor);
	virtual bool GetCanEnter_Implementation(const FActionCursorFinder& WantCursor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void AddNodeLink(FName ResultName, const FLinkedNodeInfo& Destination);
	virtual void AddNodeLink_Implementation(FName ResultName, const FLinkedNodeInfo& Destination);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void MoveExecutorToLinkedNode(const FActionCursorFinder& WantCursor, FName ResultName);
	virtual void MoveExecutorToLinkedNode_Implementation(const FActionCursorFinder& WantCursor, FName ResultName);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void MoveExecutorToNext(const FActionCursorFinder& WantCursor);
	virtual void MoveExecutorToNext_Implementation(const FActionCursorFinder& WantCursor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void MoveExecutorToCancel(const FActionCursorFinder& WantCursor);
	virtual void MoveExecutorToCancel_Implementation(const FActionCursorFinder& WantCursor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void MoveExecutorToInterrupt(const FActionCursorFinder& WantCursor, const FActionCursorFinder& InterruptCursor, UActionNode* InterruptNode);
	void MoveExecutorToInterrupt_Implementation(const FActionCursorFinder& WantCursor, const FActionCursorFinder& InterruptCursor, UActionNode* InterruptNode);


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void MoveExecutorToWantNode(const FActionCursorFinder& WantCursor, UActionNode* TargetNode, bool bIsCanceled);
	virtual void MoveExecutorToWantNode_Implementation(const FActionCursorFinder& WantCursor, UActionNode* TargetNode, bool bIsCanceled);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	int CreateSubNode(const FActionCursorFinder& WantCursor, UActionNode* TargetNode);
	int CreateSubNode_Implementation(const FActionCursorFinder& WantCursor, UActionNode* TargetNode);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	int CreateSubNode_Hit(const FActionCursorFinder& WantCursor, UActionNode* TargetNode, const FHitResult& Hit);
	int CreateSubNode_Hit_Implementation(const FActionCursorFinder& WantCursor, UActionNode* TargetNode, const FHitResult& Hit);


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void EndAllSubNodes(const FActionCursorFinder& WantCursor);
	void EndAllSubNodes_Implementation(const FActionCursorFinder& WantCursor);


	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Action")
	void OnCancel(const FActionCursorFinder& WantCursor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void OnInterrupt(const FActionCursorFinder& WantCursor, const FActionCursorFinder& InterruptCursor, UActionNode* InterruptNode);
	void OnInterrupt_Implementation(const FActionCursorFinder& WantCursor, const FActionCursorFinder& InterruptCursor, UActionNode* InterruptNode) { OnCancel(WantCursor); }

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Action")
	void OnComplete(const FActionCursorFinder& WantCursor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void ClaimExecute(const FActionCursorFinder& WantCursor);
	virtual void ClaimExecute_Implementation(const FActionCursorFinder& WantCursor) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void ClaimExecuteWithInput(const FActionCursorFinder& WantCursor, const FInputPackage& Input);
	virtual void ClaimExecuteWithInput_Implementation(const FActionCursorFinder& WantCursor, const FInputPackage& Input) { ClaimExecute(WantCursor); }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void OnActionMessage_Simple(const FActionCursorFinder& WantCursor, const FName& Message);
	void OnActionMessage_Simple_Implementation(const FActionCursorFinder& WantCursor, const FName& Message);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Action")
	void OnActionMessage_Detail(const FActionCursorFinder& WantCursor, const FName& Message, const FName& Context);


	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Action")
	void OnActionMessage_Montage(const FActionCursorFinder& WantCursor, UAnimMontage* Montage, bool bIsStart, bool bIsInterrupted);

};
