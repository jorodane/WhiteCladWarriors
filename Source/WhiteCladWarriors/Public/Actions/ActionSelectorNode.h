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
};
