// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ActionNode.generated.h"

class UActionExecutor;

/**
 * 
 */
UCLASS( Blueprintable, BlueprintType )
class WHITECLADWARRIORS_API UActionNode : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void ClaimExecute(UActionExecutor* Executor);
	virtual void ClaimExecute_Implementation(UActionExecutor* Executor) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void OnPositionInput(UActionExecutor* Executor, FVector Position);
	virtual void OnPositionInput_Implementation(UActionExecutor* Executor, FVector Position) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void OnActorInput(UActionExecutor* Executor, AActor* Actor);
	virtual void OnActorInput_Implementation(UActionExecutor* Executor, AActor* Actor) {}
};
