// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actions/ActionNode.h"
#include "ActionSelectorNode.generated.h"

/**
 * 
 */
UCLASS()
class WHITECLADWARRIORS_API UActionSelectorNode : public UActionNode
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Action", Meta = (ExposeOnSpawn = "true"))
	TObjectPtr<UActionNode> OnCanceled;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Action", Meta = (ExposeOnSpawn = "true"))
	bool bCancelable;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Action")
	bool OnPositionInput(UActionExecutor* Executor, FVector Position);

	UFUNCTION(BlueprintImplementableEvent, Category = "Action")
	bool OnActorInput(UActionExecutor* Executor, AActor* Actor);

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
	bool PositionInput(UActionExecutor* Executor, FVector Position) { return OnPositionInput(Executor, Position); }

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool ActorInput(UActionExecutor* Executor, AActor* Actor) { return OnActorInput(Executor, Actor); }

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool CancelInput(UActionExecutor* Executor);
};
