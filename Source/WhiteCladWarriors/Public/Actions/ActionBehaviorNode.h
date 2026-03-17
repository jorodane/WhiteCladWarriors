// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actions/ActionNode.h"
#include "Generals/Structs/ActionInputStructures.h"
#include "ActionBehaviorNode.generated.h"

/**
 * 
 */
UCLASS()
class WHITECLADWARRIORS_API UActionBehaviorNode : public UActionNode
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintPure, Category = "Indicator")
	FIndicatorClaim GetIndicatorClaim(const FInputClaim& TargetInput);
};
