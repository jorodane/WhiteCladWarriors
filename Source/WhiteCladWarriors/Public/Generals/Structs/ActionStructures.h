// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionStructures.generated.h"


class AActionBase;
class AOperator;
class UActionNode;
class UActionSelectorNode;
class UActionExecutor;
class UUnitActionComponent;

USTRUCT(BlueprintType)
struct FActionCursorFinder
{
	GENERATED_BODY();

	const static FActionCursorFinder None;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	TObjectPtr<AActionBase> CurrentAction = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	TObjectPtr<AOperator> CurrentOperator = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	TObjectPtr<UActionExecutor> CurrentExecutor = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	TObjectPtr<UUnitActionComponent> CurrentComponent = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	int CurrentID = 0;

	FActionCursorFinder(){}
	FActionCursorFinder(AActionBase* WantAction, AOperator* WantOperator, UActionExecutor* WantExecutor, UUnitActionComponent* WantComponent, int WantID) 
	{Set(WantAction, WantOperator, WantExecutor, WantComponent, WantID);}

	bool CheckValid() const;
	bool CheckExecutor(const UActionExecutor* WantExecutor) const;
	bool CheckOperator(const AOperator* WantOperator) const;
	bool CheckAction(const AActionBase* WantAction) const;

	void Set(AActionBase* WantAction, AOperator* WantOperator, UActionExecutor* WantExecutor, UUnitActionComponent* WantComponent, int WantID);

	void Clear();
};


USTRUCT(BlueprintType)
struct FActionTargetContainer
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Action")
	TObjectPtr<AActionBase> Action;

	UPROPERTY(BlueprintReadOnly, Category = "Action")
	TArray<UUnitActionComponent*> Components;

public:
	int GetOrder() const;
};