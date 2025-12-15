// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ActionExecutor.generated.h"

class AOperator;
class UUnitActionComponent;
class UActionNode;

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class WHITECLADWARRIORS_API UActionExecutor : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Category = "Action")
	TObjectPtr<UActionNode> CurrentNode;

	UPROPERTY(BlueprintReadOnly, Category = "Action", Meta = (ExposeOnSpawn = "true"))
	TObjectPtr<AOperator> Operator;

	UPROPERTY(BlueprintReadOnly, Category = "Action", Meta = (ExposeOnSpawn = "true"))
	TArray<UUnitActionComponent*> Target;

	TMap<FName, FVector> PositionMap;
	TMap<FName, FVector> DirectionMap;
	TMultiMap<FName, AActor*> ActorMultiMap;

public:
	UFUNCTION(BlueprintCallable, Category = "Action")
	void SetPosition(const FName& WantTag, const FVector& WantPosition);

	UFUNCTION(BlueprintPure, Category = "Action")
	FVector GetPosition(const FName& WantTag) const;

	UFUNCTION(BlueprintPure, Category = "Action")
	bool HasPosition(const FName& WantTag) const;

	UFUNCTION(BlueprintCallable, Category = "Action")
	void SetDirection(const FName& WantTag, const FVector& WantDirection);

	UFUNCTION(BlueprintPure, Category = "Action")
	FVector GetDirection(const FName& WantTag) const;

	UFUNCTION(BlueprintPure, Category = "Action")
	bool HasDirection(const FName& WantTag) const;

	UFUNCTION(BlueprintCallable, Category = "Action")
	void AddActor(const FName& WantTag, AActor* WantActor);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void RemoveActor(const FName& WantTag, AActor* WantActor);

	UFUNCTION(BlueprintPure, Category = "Action")
	AActor* GetActor(const FName& WantTag) const;

	UFUNCTION(BlueprintPure, Category = "Action")
	TArray<AActor*> GetActorArray(const FName& WantTag) const;

	UFUNCTION(BlueprintCallable, Category = "Action")
	void EnterNode(UActionNode* TargetNode);

	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	void OnNodeEnter(UActionNode* NewNode);
	void OnNodeEnter_Implementation(UActionNode* NewNode) {};

	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	void OnNodeEnd(UActionNode* LastNode);
	void OnNodeEnd_Implementation(UActionNode* LastNode) {};

	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	void OnNodeMove(UActionNode* OldNode, UActionNode* NewNode);
	void OnNodeMove_Implementation(UActionNode* OldNode, UActionNode* NewNode) {};
};
