// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Generals/Structs/InputPackage.h"
#include "Generals/Structs/ActionInputStructures.h"
#include "Actions/ActionNode.h"
#include "ActionSelectorNode.generated.h"

class UActionNode;
class UActionBehaviorNode;
class AActionIndicatorShowerBase;
/**
 * 
 */

UCLASS()
class WHITECLADWARRIORS_API UActionSelectorNode : public UActionNode
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	FText InputFailReason;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Action", Meta = (ExposeOnSpawn = "true"))
	TArray<FSelectorInput> InputTypes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Action", Meta = (ExposeOnSpawn = "true"))
	TArray<UActionBehaviorNode*> IndicatorNodes;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Action")
	bool OnReceiveInput(const FActionCursorFinder& WantCursor, const FInputPackage& Input);

	UFUNCTION(BlueprintImplementableEvent, Category = "Action")
	bool OnReceivePosition(const FActionCursorFinder& WantCursor, const FName& TargetTag, const FVector& TargetPosition);

	UFUNCTION(BlueprintImplementableEvent, Category = "Action")
	bool OnReceiveDirection(const FActionCursorFinder& WantCursor, const FName& TargetTag, const FVector& TargetDirection);

	UFUNCTION(BlueprintImplementableEvent, Category = "Action")
	bool OnReceiveActor(const FActionCursorFinder& WantCursor, const FName& TargetTag, AActor* TargetActor);

	//UFUNCTION(BlueprintImplementableEvent, Category = "Action")
	//bool OnReceiveActorArray(const FActionCursorFinder& WantCursor, const FName& TargetTag, TArray<AActor*> TargetActors);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Action")
	bool CheckPosition(const FActionCursorFinder& WantCursor, const FName& TargetTag, const FVector& TargetPosition);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Action")
	bool CheckDirection(const FActionCursorFinder& WantCursor, const FName& TargetTag, const FVector& TargetDirection);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Action")
	bool CheckActor(const FActionCursorFinder& WantCursor, const FName& TargetTag, AActor* TargetActor);

	//UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Action")
	//bool CheckActorArray(const FActionCursorFinder& WantCursor, const FName& TargetTag, TArray<AActor*> TargetActors);

	UFUNCTION(BlueprintImplementableEvent, Category = "Action")
	bool OnCancelInput(const FActionCursorFinder& WantCursor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	bool CompleteInput(const FActionCursorFinder& WantCursor);
	bool CompleteInput_Implementation(const FActionCursorFinder& WantCursor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	void FailInput(const FActionCursorFinder& WantCursor);
	void FailInput_Implementation(const FActionCursorFinder& WantCursor);

public:
	UFUNCTION(BlueprintCallable, Category = "Action")
	bool ReceiveInput(const FActionCursorFinder& WantCursor, const FInputPackage& Input);

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool CancelInput(const FActionCursorFinder& WantCursor);

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool CheckInput(const FActionCursorFinder& WantCursor, const FInputPackage& Input, EInputType& ResultType, FText& FailReason);

	UFUNCTION(BlueprintImplementableEvent, BlueprintPure, Category = "Action")
	FInputClaim GetInputClaim(const TArray<UUnitActionComponent*>& TargetComponent, const AActionBase* TargetAction, UActionExecutor* Executor) const;

	UFUNCTION(BlueprintPure, Category = "Indicator")
	TMap<UActionBehaviorNode*, FIndicatorClaim> GetIndicatorClaim(const FInputClaim& TargetInput);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Indicator")
	void UpdateIndicator(const FInputClaim& TargetInput, const TArray<AActionIndicatorShowerBase*>& TargetIndicators);
};
