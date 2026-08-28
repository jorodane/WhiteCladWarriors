// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actions/Executables/ActionBehaviorNode.h"
#include "Generals/Structs/ActionStructures.h"
#include "ActionDelayNode.generated.h"

USTRUCT(BlueprintType)
struct FDelayInfo
{
	GENERATED_BODY()

	FTimerHandle Handle;

	UPROPERTY(BlueprintReadOnly, Category = "ActionDelay")
	int CurrentRepeateCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "ActionDelay")
	int MaxRepeatCount = 1;
};
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class WHITECLADWARRIORS_API UActionDelayNode : public UActionBehaviorNode
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(Transient)
	TMap<FActionCursorFinder, FDelayInfo> TimerList;

public:
	UFUNCTION(BlueprintCallable, Category = "ActionDelay")
	void StartTimer(const FActionCursorFinder& BaseCursor, float WantTime, int RepeatCount = 1);
	UFUNCTION(BlueprintImplementableEvent, Category = "ActionDelay")
	void OnStartTimer(const FActionCursorFinder& BaseCursor);

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
	void ActivateTimer(const FActionCursorFinder& BaseCursor);

	UFUNCTION(BlueprintImplementableEvent, Category = "ActionDelay")
	void OnActivated(const FActionCursorFinder& BaseCursor, int CurrentIndex, int MaxCount);

	UFUNCTION(BlueprintCallable, Category = "ActionDelay")
	void ClearAll();
};
