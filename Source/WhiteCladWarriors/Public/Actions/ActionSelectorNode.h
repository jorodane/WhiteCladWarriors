// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Generals/Structs/InputPackage.h"
#include "Actions/ActionNode.h"
#include "ActionSelectorNode.generated.h"

struct FInputClaim;
/**
 * 
 */
UENUM(BlueprintType)
enum class EInputType : uint8
{
	Position, Direction, SingleTarget, MultiTarget,
};

UCLASS()
class WHITECLADWARRIORS_API UActionSelectorNode : public UActionNode
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Action")
	bool OnReceiveInput(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID, const FInputPackage& Input);

	UFUNCTION(BlueprintImplementableEvent, Category = "Action")
	bool OnCancelInput(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	bool CompleteInput(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID);
	bool CompleteInput_Implementation(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	void FailInput(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID);
	void FailInput_Implementation(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID);

public:
	UFUNCTION(BlueprintCallable, Category = "Action")
	bool ReceiveInput(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID, const FInputPackage& Input) { return OnReceiveInput(Executor, TargetComponent, ID, Input); }

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool CancelInput(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID);

	UFUNCTION(BlueprintImplementableEvent, BlueprintPure, Category = "Action")
	FInputClaim GetInputClaim(const TArray<UUnitActionComponent*>& TargetComponent, const AActionBase* TargetAction, UActionExecutor* Executor) const;
};
