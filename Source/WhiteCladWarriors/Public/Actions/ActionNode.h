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
UCLASS( Blueprintable, BlueprintType )
class WHITECLADWARRIORS_API UActionNode : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Action", Meta = (ExposeOnSpawn = "true"))
	TObjectPtr<UActionNode> NextNode;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Action", Meta = (ExposeOnSpawn = "true"))
	TObjectPtr<UActionNode> BlockedNode;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Action", Meta = (ExposeOnSpawn = "true"))
	TMap<FName, UActionNode*> LinkedNodes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Action", Meta = (ExposeOnSpawn = "true"))
	bool bIsMainAction = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Action", Meta = (ExposeOnSpawn = "true"))
	bool bIsCancelable = false;
	
public:
	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Action")
	bool GetCanEnter(UActionExecutor* Executor, UUnitActionComponent* TargetComponent);
	virtual bool GetCanEnter_Implementation(UActionExecutor* Executor, UUnitActionComponent* TargetComponent);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void AddNodeLink(FName ResultName, UActionNode* Destination);
	virtual void AddNodeLink_Implementation(FName ResultName, UActionNode* Destination);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void MoveExecutorToLinkedNode(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, FName ResultName);
	virtual void MoveExecutorToLinkedNode_Implementation(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, FName ResultName);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void MoveExecutorToNext(UActionExecutor* Executor, UUnitActionComponent* TargetComponent);
	virtual void MoveExecutorToNext_Implementation(UActionExecutor* Executor, UUnitActionComponent* TargetComponent);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void ClaimExecute(UActionExecutor* Executor, UUnitActionComponent* TargetComponent);
	virtual void ClaimExecute_Implementation(UActionExecutor* Executor, UUnitActionComponent* TargetComponent) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void ClaimExecuteWithInput(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, const FInputPackage& Input);
	virtual void ClaimExecuteWithInput_Implementation(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, const FInputPackage& Input) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void OnActionMessage_Simple(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, const FName& Message);
	void OnActionMessage_Simple_Implementation(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, const FName& Message);
};
