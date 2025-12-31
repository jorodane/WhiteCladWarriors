// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ActionExecutor.generated.h"

class AOperator;
class UUnitActionComponent;
class UActionSelectorNode;
class UActionNode;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionMessage_Simple, AUnitBase*, From);

/**
 * 
 */
USTRUCT(BlueprintType)
struct FActiveNodeInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Action")
	TObjectPtr<UActionNode> CurrentNode;

	UPROPERTY(BlueprintReadOnly, Category = "Action")
	bool bIsMainNode;

	void SetNode(UActionNode* Node);

	FActiveNodeInfo() { }
	FActiveNodeInfo(UActionNode* Node) { SetNode(Node); }
};


UCLASS(Blueprintable, BlueprintType)
class WHITECLADWARRIORS_API UActionExecutor : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Action", Meta = (ExposeOnSpawn = "true"))
	TObjectPtr<AOperator> Operator;

	UPROPERTY(BlueprintReadOnly, Category = "Action")
	TMap<UUnitActionComponent*, FActiveNodeInfo> ComponentMap;

	TMap<FName, FVector> PositionMap;
	TMap<TPair<UUnitActionComponent*, FName>, FVector> DirectionMap;
	TMultiMap<FName, AActor*> ActorMultiMap;


public:
	UFUNCTION(BlueprintCallable, Category = "Action")
	void SetActionMessage_Simple(UUnitActionComponent* From, const FName& Message);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void SetPosition(FName WantTag, const FVector& WantPosition);

	UFUNCTION(BlueprintPure, Category = "Action")
	FVector GetPosition(FName WantTag) const;

	UFUNCTION(BlueprintPure, Category = "Action")
	bool HasPosition(FName WantTag) const;

	UFUNCTION(BlueprintCallable, Category = "Action")
	void SetDirection(FName WantTag, UUnitActionComponent* WantComponent, const FVector& WantDirection);

	UFUNCTION(BlueprintPure, Category = "Action")
	FVector GetDirection(FName WantTag, UUnitActionComponent* WantComponent) const;

	UFUNCTION(BlueprintPure, Category = "Action")
	bool HasDirection(FName WantTag, UUnitActionComponent* WantComponent) const;

	UFUNCTION(BlueprintCallable, Category = "Action")
	void AddActor(FName WantTag, AActor* WantActor);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void RemoveActor(FName WantTag, AActor* WantActor);

	UFUNCTION(BlueprintPure, Category = "Action")
	AActor* GetActor(FName WantTag) const;

	UFUNCTION(BlueprintPure, Category = "Action")
	TArray<AActor*> GetActorArray(FName WantTag) const;

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool SetInput(UUnitActionComponent* WantComponent, UActionSelectorNode* WantNode, const FInputPackage& WantInput);

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool SetInputArray(TArray<UUnitActionComponent*> WantComponent, UActionSelectorNode* WantNode, const FInputPackage& WantInput);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void EnterNode(UUnitActionComponent* TargetComponent, UActionNode* TargetNode, int RecursiveDepth = 12);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void EndNode(UUnitActionComponent* TargetComponent, UActionNode* OldNode);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void AddComponentToMap(UUnitActionComponent* TargetComponent, UActionNode* StartNode);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void AddComponentBaseToMap(UUnitComponentBase* TargetComponent, UActionNode* StartNode);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void RemoveComponentFromMap(UUnitActionComponent* TargetComponent);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void RemoveComponentBaseFromMap(UUnitComponentBase* TargetComponent);


	UFUNCTION(BlueprintCallable, Category = "Action")
	void CheckComponentMap();

public:
	UFUNCTION(BlueprintCallable, Category = "Action")
	static UActionExecutor* CreateExecutor(AOperator* TargetOperator, TArray<UUnitActionComponent*> TargetComponents, UActionNode* StartNode);
};
