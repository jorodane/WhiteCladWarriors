// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Generals/Structs/ActionInputStructures.h"
#include "ActionIndicatorBase.generated.h"

class AOperator;
class UActionSelectorNode;
class UActionExecutor;
class UActionNode;
class UUnitActionComponent;

UCLASS(Blueprintable, BlueprintType)
class UActionIndicatorBase : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Indicator")
	AOperator* OwnerOperator;

	UPROPERTY(BlueprintReadOnly, Category = "Indicator")
	TObjectPtr<UActionExecutor> CurrentExecutor;

	UPROPERTY(BlueprintReadOnly, Category = "Indicator")
	TObjectPtr<UActionSelectorNode> CurrentNode;

	UPROPERTY(BlueprintReadOnly, Category = "Indicator")
	TArray<UUnitActionComponent*> CurrentComponents;

	UPROPERTY(BlueprintReadWrite, Category = "Indicator")
	bool bIsActivated = false;

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Indicator")
	void SetOwner(AOperator* NewOperator);
	void SetOwner_Implementation(AOperator* NewOperator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Indicator")
	void SetIndicator(UActionExecutor* TargetExecutor, const TArray<UUnitActionComponent*>& TargetComponents, UActionSelectorNode* StartNode);
	void SetIndicator_Implementation(UActionExecutor* TargetExecutor, const TArray<UUnitActionComponent*>& TargetComponents, UActionSelectorNode* StartNode);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Indicator")
	void SetVisible();
	void SetVisible_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Indicator")
	void SetInvisible();
	void SetInvisible_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Indicator")
	void ReceiveInputClaim(const FInputClaim& NewClaim, bool ValidClaim);
	void ReceiveInputClaim_Implementation(const FInputClaim& NewClaim, bool ValidClaim);
};
