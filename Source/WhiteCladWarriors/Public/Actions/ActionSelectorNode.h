// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Generals/Structs/InputPackage.h"
#include "Actions/ActionNode.h"
#include "ActionSelectorNode.generated.h"

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
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Action", Meta = (ExposeOnSpawn = "true"))
	TObjectPtr<UActionNode> OnFailed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Action", Meta = (ExposeOnSpawn = "true"))
	TObjectPtr<UActionNode> OnCanceled;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Action", Meta = (ExposeOnSpawn = "true"))
	bool bCancelable;

public:
	UActionSelectorNode();

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Action")
	bool OnReceiveInput(UActionExecutor* Executor, const FInputPackage& Input);

	UFUNCTION(BlueprintImplementableEvent, Category = "Action")
	bool OnCancelInput(UActionExecutor* Executor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	bool CompleteInput(UActionExecutor* Executor);
	bool CompleteInput_Implementation(UActionExecutor* Executor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	void FailInput(UActionExecutor* Executor);
	void FailInput_Implementation(UActionExecutor* Executor);

public:
	UFUNCTION(BlueprintCallable, Category = "Action")
	bool ReceiveInput(UActionExecutor* Executor, const FInputPackage& Input) { return OnReceiveInput(Executor, Input); }

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool CancelInput(UActionExecutor* Executor);
};
