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
	bool bCancelable;

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void OnPositionInput(UActionExecutor* Executor, FVector Position);
	virtual void OnPositionInput_Implementation(UActionExecutor* Executor, FVector Position) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void OnActorInput(UActionExecutor* Executor, AActor* Actor);
	virtual void OnActorInput_Implementation(UActionExecutor* Executor, AActor* Actor) {}
};
