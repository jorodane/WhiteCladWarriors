// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Generals/Structs/InputPackage.h"
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
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action", Meta = (ExposeOnSpawn = "true"))
	TObjectPtr<UActionNode> NextNode;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action", Meta = (ExposeOnSpawn = "true"))
	TObjectPtr<UActionNode> CanceledNode;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action", Meta = (ExposeOnSpawn = "true"))
	TObjectPtr<UActionNode> BlockedNode;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action", Meta = (ExposeOnSpawn = "true"))
	TMap<FName, FLinkedNodeInfo> LinkedNodes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Action", Meta = (ExposeOnSpawn = "true"))
	bool bIsMainAction = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Action", Meta = (ExposeOnSpawn = "true"))
	bool bIsCancelable = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Action", Meta = (ExposeOnSpawn = "true"))
	bool bIsStopMovementOnStart = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Action", Meta = (ExposeOnSpawn = "true"))
	bool bIsStopMovementOnEnd = false;
	
public:
	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Action")
	bool GetCanEnter(UActionExecutor* Executor, UUnitActionComponent* TargetComponent);
	virtual bool GetCanEnter_Implementation(UActionExecutor* Executor, UUnitActionComponent* TargetComponent);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void AddNodeLink(FName ResultName, const FLinkedNodeInfo& Destination);
	virtual void AddNodeLink_Implementation(FName ResultName, const FLinkedNodeInfo& Destination);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void MoveExecutorToLinkedNode(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID, FName ResultName);
	virtual void MoveExecutorToLinkedNode_Implementation(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID, FName ResultName);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void MoveExecutorToNext(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID);
	virtual void MoveExecutorToNext_Implementation(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void MoveExecutorToCancel(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID);
	virtual void MoveExecutorToCancel_Implementation(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void ClaimCancel(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID, bool bWantStopMovement);
	virtual void ClaimCancel_Implementation(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID, bool bWantStopMovement);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void ClaimExecute(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID);
	virtual void ClaimExecute_Implementation(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void ClaimExecuteWithInput(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID, const FInputPackage& Input);
	virtual void ClaimExecuteWithInput_Implementation(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID, const FInputPackage& Input) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void OnActionMessage_Simple(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID, const FName& Message);
	void OnActionMessage_Simple_Implementation(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID, const FName& Message);
};
