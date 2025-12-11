// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Generals/Structs/InputPackage.h"
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
	void ClaimExecuteWithInput(UActionExecutor* Executor, const FInputPackage& Input);
	virtual void ClaimExecuteWithInput_Implementation(UActionExecutor* Executor, const FInputPackage& Input) {}
};
