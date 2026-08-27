// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actions/Executables/ActionNode.h"
#include "Generals/Structs/ActionStructures.h"
#include "ActionDelayNode.generated.h"
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class WHITECLADWARRIORS_API UActionDelayNode : public UActionNode
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(Transient)
	TMap<FActionCursorFinder, FTimerHandle> TimerList;

public:
	UFUNCTION(BlueprintCallable, Category = "ActionDelay")
	void StartTimer(const FActionCursorFinder& BaseCursor, float WantTime);
	UFUNCTION(BlueprintImplementableEvent, Category = "ActionDelay")
	void OnStartTimer(const FActionCursorFinder& BaseCursor, float WantTime);

	UFUNCTION(BlueprintCallable, Category = "ActionDelay")
	void CancelTimer(const FActionCursorFinder& BaseCursor);
	UFUNCTION(BlueprintImplementableEvent, Category = "ActionDelay")
	void OnCancelTimer(const FActionCursorFinder& BaseCursor);

	UFUNCTION(BlueprintImplementableEvent, Category = "ActionDelay")
	void OnBlockTimer(const FActionCursorFinder& BaseCursor);

	UFUNCTION(BlueprintCallable, Category = "ActionDelay")
	void FinishTimer(const FActionCursorFinder& BaseCursor);
	UFUNCTION(BlueprintImplementableEvent, Category = "ActionDelay")
	void OnFinishTimer(const FActionCursorFinder& BaseCursor);

	UFUNCTION(BlueprintCallable, Category = "ActionDelay")
	void ClearAll();
};
