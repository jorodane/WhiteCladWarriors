// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ActionExecutor.generated.h"

class AOperator;
class UUnitActionComponent;
class UActionNode;

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class WHITECLADWARRIORS_API UActionExecutor : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Category = "Action")
	TObjectPtr<UActionNode> CurrentNode;

	UPROPERTY(BlueprintReadOnly, Category = "Action", Meta = (ExposeOnSpawn = "true"))
	TObjectPtr<AOperator> Operator;

	UPROPERTY(BlueprintReadOnly, Category = "Action", Meta = (ExposeOnSpawn = "true"))
	TArray<UUnitActionComponent*> Target;

public:
	UFUNCTION(BlueprintCallable, Category = "Action")
	void EnterNode(UActionNode* TargetNode);

	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	void OnNodeEnter(UActionNode* NewNode);
	void OnNodeEnter_Implementation(UActionNode* NewNode) {};

	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	void OnNodeEnd(UActionNode* LastNode);
	void OnNodeEnd_Implementation(UActionNode* LastNode) {};

	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	void OnNodeMove(UActionNode* OldNode, UActionNode* NewNode);
	void OnNodeMove_Implementation(UActionNode* OldNode, UActionNode* NewNode) {};
};
