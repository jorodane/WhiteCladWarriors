// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Generals/Structs/InputPackage.h"
#include "Actions/ActionNode.h"
#include "ActionSelectorNode.generated.h"

class UActionNode;
struct FInputClaim;
/**
 * 
 */
UENUM(BlueprintType)
enum class EInputType : uint8
{
	Position, Direction, SingleTarget, MultiTarget,
};

USTRUCT(BlueprintType)
struct FSelectorInput
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	EInputType Type;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	FName Tag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	UActionNode* OnInputAccepted;
};

UCLASS()
class WHITECLADWARRIORS_API UActionSelectorNode : public UActionNode
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	FText InputFailReason;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action", Meta = (ExposeOnSpawn = "true"))
	TArray<FSelectorInput> InputTypes;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Action")
	bool OnReceiveInput(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID, const FInputPackage& Input);

	UFUNCTION(BlueprintImplementableEvent, Category = "Action")
	bool OnReceivePosition(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID, const FName& TargetTag, const FVector& TargetPosition);

	UFUNCTION(BlueprintImplementableEvent, Category = "Action")
	bool OnReceiveDirection(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID, const FName& TargetTag, const FVector& TargetDirection);

	UFUNCTION(BlueprintImplementableEvent, Category = "Action")
	bool OnReceiveActor(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID, const FName& TargetTag, AActor* TargetActor);

	//UFUNCTION(BlueprintImplementableEvent, Category = "Action")
	//bool OnReceiveActorArray(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID, const FName& TargetTag, TArray<AActor*> TargetActors);

	UFUNCTION(BlueprintImplementableEvent, Category = "Action")
	void OnCheckEffectSpawn(bool Result, UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID, const FInputPackage& Input, const EInputType& ResultType, const FText& FailReason);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Action")
	bool CheckPosition(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID, const FName& TargetTag, const FVector& TargetPosition);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Action")
	bool CheckDirection(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID, const FName& TargetTag, const FVector& TargetDirection);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Action")
	bool CheckActor(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID, const FName& TargetTag, AActor* TargetActor);

	//UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Action")
	//bool CheckActorArray(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID, const FName& TargetTag, TArray<AActor*> TargetActors);

	UFUNCTION(BlueprintImplementableEvent, Category = "Action")
	bool OnCancelInput(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	bool CompleteInput(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID);
	bool CompleteInput_Implementation(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	void FailInput(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID);
	void FailInput_Implementation(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID);

public:
	UFUNCTION(BlueprintCallable, Category = "Action")
	bool ReceiveInput(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID, const FInputPackage& Input);

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool CancelInput(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID);

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool CheckInput(UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID, const FInputPackage& Input, EInputType& ResultType, FText& FailReason);

	UFUNCTION(BlueprintImplementableEvent, BlueprintPure, Category = "Action")
	FInputClaim GetInputClaim(const TArray<UUnitActionComponent*>& TargetComponent, const AActionBase* TargetAction, UActionExecutor* Executor) const;
};
