// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Generals/Structs/ActionInputStructures.h"
#include "ActionIndicatorBase.generated.h"

class AOperator;
class UPoolComponent;
class UActionSelectorNode;
class UActionBehaviorNode;
class AActionIndicatorShowerBase;
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
	TArray<UUnitActionComponent*> CurrentComponents;

	UPROPERTY(BlueprintReadOnly, Category = "Indicator")
	TObjectPtr<UActionSelectorNode> CurrentNode;

	UPROPERTY(BlueprintReadOnly, Category = "Indicator")
	FInputClaim CurrentClaim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pool")
	TMap<EInputIndicatorType, TSubclassOf<AActionIndicatorShowerBase>> PoolDefaultClassMap;

	UPROPERTY(BlueprintReadOnly, Category = "Pool")
	TMap<EInputIndicatorType, UPoolComponent*> PoolComponentMap;

	TMap<UActionBehaviorNode*, TSet<AActionIndicatorShowerBase*>> ShowerActiveMap;

	UPROPERTY(BlueprintReadWrite, Category = "Indicator")
	bool bIsActivated = false;

public:
	void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Indicator")
	void SetOwnerOperator(AOperator* NewOperator);
	void SetOwnerOperator_Implementation(AOperator* NewOperator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Indicator")
	void SetVisible();
	void SetVisible_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Indicator")
	void SetInvisible();
	void SetInvisible_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Indicator")
	void ReceiveInputClaim(const FInputClaim& NewClaim, bool ValidClaim);
	void ReceiveInputClaim_Implementation(const FInputClaim& NewClaim, bool ValidClaim);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Indicator")
	void UpdateShower();
	void UpdateShower_Implementation();
};
