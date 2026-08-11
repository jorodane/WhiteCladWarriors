// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actions/Executables/ActionBehaviorNode.h"
#include "Generals/Structs/ActionStructures.h"
#include "ActionSpawnNode.generated.h"

/**
 * 
 */
UCLASS()
class WHITECLADWARRIORS_API UActionSpawnNode : public UActionBehaviorNode
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Spawn")
	void InstanceRegistration(AActor* Instance, const FActionCursorFinder& BaseCursor);
	void InstanceRegistration_Implementation(AActor* Instance, const FActionCursorFinder& BaseCursor);
};
