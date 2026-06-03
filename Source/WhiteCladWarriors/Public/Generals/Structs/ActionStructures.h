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
	int64 CurrentExecutorID = -1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	TObjectPtr<UUnitActionComponent> CurrentComponent = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	int CurrentID = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	bool bAsSubNode = false;

	FActionCursorFinder(){}
	FActionCursorFinder(AActionBase* WantAction, AOperator* WantOperator, int64 WantExecutorID, UUnitActionComponent* WantComponent, int WantID, bool bAsSubNode)
	{Set(WantAction, WantOperator, WantExecutorID, WantComponent, WantID, bAsSubNode);}

	UActionExecutor* GetExecutor() const;

	bool CheckValid() const;
	bool CheckIsMainNode() const { return CurrentID == 0 && !bAsSubNode; }
	bool CheckExecutor(const int64 WantExecutorID) const;
	bool CheckOperator(const AOperator* WantOperator) const;
	bool CheckAction(const AActionBase* WantAction) const;

	void Set(AActionBase* WantAction, AOperator* WantOperator, int64 WantExecutorID, UUnitActionComponent* WantComponent, int WantID, bool bWantSubNode);

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
	FActionTargetContainer() : Action(nullptr) {};
	FActionTargetContainer(AActionBase* NewAction) { Action = NewAction; };

public:
	int GetOrder() const;
};



USTRUCT(BlueprintType)
struct FActionExecuteSettingContainer
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	bool bIsMainAction = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	bool bIsCancelable = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	bool bIsResetIntent = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	bool bIsStopMovementOnStart = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	bool bIsStopActionMontageOnStart = true;

public:
	void Clear() 
	{
		bIsMainAction = false;
		bIsCancelable = true;
		bIsResetIntent = true;
		bIsStopMovementOnStart = false;
		bIsStopActionMontageOnStart = true;
	}
};

USTRUCT(BlueprintType)
struct FActionIntentContainer
{
	GENERATED_BODY()

public:
	static const FActionIntentContainer None;

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	AOperator* OrderedOperator = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	AActor* TargetActor = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	FName IntentType = NAME_None;

public:
	void Clear() {*this = None;}

	bool operator==(const FActionIntentContainer& other) const
	{
		return
			OrderedOperator == other.OrderedOperator &&
			TargetActor == other.TargetActor &&
			IntentType == other.IntentType;
	}
};

USTRUCT(BlueprintType)
struct FMainActionInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	FActionCursorFinder Cursor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	FActionExecuteSettingContainer Settings;

	void Clear();

	void Clear(int64 OldExecutorID);

	void Set(const FActionCursorFinder& WantCursor, const FActionExecuteSettingContainer& WantSetting);

	void SetActionMessage_Simple(FName Message);

	bool Cancel();

	bool End();

	bool CheckValid() const;
};