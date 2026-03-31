// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Generals/Structs/InputPackage.h"
#include "ActionBase.generated.h"

class UActionNode;
class UActionSelectorNode;
class AOperator;

UCLASS( Blueprintable, BlueprintType )
class WHITECLADWARRIORS_API AActionBase : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Action")
	TObjectPtr<UActionNode> RootNode = nullptr;

protected:
	UPROPERTY(EditAnywhere, Category = "Action")
	FKey ActionHotKey;

	UPROPERTY(EditAnywhere, Category = "Action")
	FName ActionNameDefine;

	UPROPERTY(EditAnywhere, Category = "Action")
	FText ActionNameUI;

	UPROPERTY(EditAnywhere, Category = "Action")
	FText ActionNameUIWithKeyFormat;

	UPROPERTY(EditAnywhere, Category = "Action")
	FSlateBrush ActionIcon;

	UPROPERTY(EditAnywhere, Category = "Action")
	float ActionCooldown;

	UPROPERTY(EditAnywhere, Category = "Action")
	int ActionMaxStack = 1;

	UPROPERTY(EditAnywhere, Category = "Action")
	int UIOrder = 100;

	UPROPERTY(EditAnywhere, Category = "Action")
	bool bIsMainAction = true;

	UPROPERTY(EditAnywhere, Category = "Action")
	bool bIsSmartKey = false;

	UPROPERTY(EditAnywhere, Category = "Action")
	bool bShowStack = false;

	UPROPERTY(EditAnywhere, Category = "Action")
	bool bShowHotKey = false;

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Action")
	void SpawnCheckEffect(bool Result, AOperator* Operator, const FInputPackage& Input, EInputType ResultType, const FText& FailReason) const;


	UFUNCTION(BlueprintPure, Category = "Action")
	inline FKey		GetHotKey() { return ActionHotKey; }
	UFUNCTION(BlueprintCallable, Category = "Action")
	inline FKey		SetHotKey(FKey Value) { return ActionHotKey = Value; }


	UFUNCTION(BlueprintPure, Category = "Action")
	inline FName		GetActionNameDefine() { return ActionNameDefine; }
	UFUNCTION(BlueprintCallable, Category = "Action")
	inline FName		SetActionNameDefine(FName Value) { return ActionNameDefine = Value; }

	UFUNCTION(BlueprintPure, Category = "Action")
	inline FText		GetActionNameUI()				{ return ActionNameUI;}
	UFUNCTION(BlueprintPure, Category = "Action")
	inline FText		GetActionNameUIWithKey();
	UFUNCTION(BlueprintCallable, Category = "Action")
	inline FText		SetActionNameUI(FText Value)	{ return ActionNameUI = Value; }

	UFUNCTION(BlueprintPure, Category = "Action")
	inline FSlateBrush	GetActionIcon()				{ return ActionIcon; }
	UFUNCTION(BlueprintCallable, Category = "Action")
	inline FSlateBrush	SetActionIcon(FSlateBrush Value) { return ActionIcon = Value; }

	UFUNCTION(BlueprintPure, Category = "Action")
	inline float		GetActionCooldown()			{ return ActionCooldown; }
	UFUNCTION(BlueprintCallable, Category = "Action")
	inline float		SetActionCooldown(float Value)	{ return ActionCooldown = Value;}

	UFUNCTION(BlueprintPure, Category = "Action")
	inline int			GetActionMaxStack()			{ return ActionMaxStack; }
	UFUNCTION(BlueprintCallable, Category = "Action")
	inline int			SetActionMaxStack(int Value) { return ActionMaxStack = Value; }

	UFUNCTION(BlueprintPure, Category = "Action")
	inline int			GetUIOrder() { return UIOrder; }
	UFUNCTION(BlueprintCallable, Category = "Action")
	inline int			SetUIOrder(int Value) { return UIOrder = Value; }

	UFUNCTION(BlueprintPure, Category = "Action")
	inline bool			GetIsSmartKey() const { return bIsSmartKey; }
	UFUNCTION(BlueprintCallable, Category = "Action")
	inline bool			SetIsSmartKey(bool Value) { return bIsSmartKey = Value; }

	UFUNCTION(BlueprintPure, Category = "Action")
	inline bool			GetShowStack() { return bShowStack; }
	UFUNCTION(BlueprintCallable, Category = "Action")
	inline bool			SetShowStack(bool Value) { return bShowStack = Value; }

	UFUNCTION(BlueprintPure, Category = "Action")
	inline bool			GetShowHotKey() { return bShowHotKey; }
	UFUNCTION(BlueprintCallable, Category = "Action")
	inline bool			SetShowHotKey(bool Value) { return bShowHotKey = Value; }

	UFUNCTION(BlueprintImplementableEvent, BlueprintPure, Category = "Action")
	int					GetSimpleActionOrder(const FInputPackage& CurrentInput, UUnitActionComponent* CurrentTarget);

	UFUNCTION(BlueprintPure, Category = "Action")
	inline bool			IsRootNodeSelector(UActionSelectorNode*& AsSelectorNode) const;

	UFUNCTION(BlueprintPure, Category = "Action")
	inline bool			IsExecutable(UUnitActionComponent* CurrentTarget) const { return true; };

	UFUNCTION(BlueprintPure, Category = "Action")
	bool			IsNeedInputForStart(FInputClaim& TriggerInput, const TArray<UUnitActionComponent*>& TargetComponent) const;

	UFUNCTION(BlueprintPure, Category = "Action")
	bool			IsNeedInputForStartCheck() const;

	UFUNCTION(BlueprintPure, Category = "Action")
	bool	IsValidInputForStart(const FInputPackage& Input, AOperator* Operator, const TArray<UUnitActionComponent*>& TargetComponent, TArray<bool>& ResultEachComponent, EInputType& TypeResult, FText& ReasonResult);

	UFUNCTION(BlueprintPure, Category = "Action")
	inline TArray<UUnitActionComponent*> GetExecutableArray(const TArray<UUnitActionComponent*>& TargetComponents) const { return TargetComponents; };

	UFUNCTION(BlueprintPure, Category = "Action")
	inline UActionSelectorNode*		RootNodeAsSelector() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	UActionExecutor* ExecuteAction(AOperator* TargetOperator, const TArray<UUnitActionComponent*>& TargetComponents) const;
	UActionExecutor* ExecuteAction_Implementation(AOperator* TargetOperator, const TArray<UUnitActionComponent*>& TargetComponents);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	UActionExecutor* ExecuteActionWithInput(AOperator* TargetOperator, const TArray<UUnitActionComponent*>& TargetComponents, const FInputPackage& Input) const;
	UActionExecutor* ExecuteActionWithInput_Implementation(AOperator* TargetOperator, const TArray<UUnitActionComponent*>& TargetComponents, const FInputPackage& Input);

};
