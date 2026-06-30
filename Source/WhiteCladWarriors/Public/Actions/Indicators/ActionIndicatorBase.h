// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Generals/Structs/ActionInputStructures.h"
#include "ActionIndicatorBase.generated.h"

class AOperator;
class AActionBase;
class UActionSelectorNode;
class UActionBehaviorNode;
class AActionIndicatorShowerBase;
class UActionExecutor;
class UActionNode;
class UPoolExpandableComponent;
class UUnitActionContainer;
class UUnitActionComponent;

UCLASS(Blueprintable, BlueprintType)
class UActionIndicatorBase : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Indicator")
	AOperator* OwnerOperator;

	UPROPERTY(BlueprintReadOnly, Category = "Indicator")
	int64 CurrentExecutorID;

	UPROPERTY(BlueprintReadOnly, Category = "Indicator")
	TArray<UUnitActionComponent*> CurrentComponents;

	UPROPERTY(BlueprintReadOnly, Category = "Indicator")
	TObjectPtr<UActionSelectorNode> CurrentNode;

	UPROPERTY(BlueprintReadOnly, Category = "Indicator")
	FInputClaim CurrentClaim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pool")
	TMap<EInputIndicatorType, TSubclassOf<AActionIndicatorShowerBase>> PoolDefaultClassMap;

	UPROPERTY(BlueprintReadOnly, Category = "Pool")
	TMap<EInputIndicatorType, UPoolExpandableComponent*> PoolComponentMap;

	TMap<UActionBehaviorNode*, TArray<AActionIndicatorShowerBase*>> ShowerActiveMap;

	UPROPERTY(BlueprintReadWrite, Category = "Indicator")
	bool bIsActivated = false;

	UPROPERTY(BlueprintReadWrite, Category = "Indicator")
	bool bIsTriggerByIcon = false;

public:
	UActionIndicatorBase();

public:
	void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Indicator")
	void ResetIndicator();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Indicator")
	void SetOwnerOperator(AOperator* NewOperator);
	void SetOwnerOperator_Implementation(AOperator* NewOperator);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Indicator")
	void ClearShower();
	void ClearShower_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Indicator")
	void ReceiveInputClaim(const FInputClaim& NewClaim, bool ValidClaim, bool TriggerByIcon);
	void ReceiveInputClaim_Implementation(const FInputClaim& NewClaim, bool ValidClaim, bool TriggerByIcon);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Indicator")
	void ReceiveAction(AActionBase* NewAction);
	void ReceiveAction_Implementation(AActionBase* NewAction);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Indicator")
	void ReceiveActionContainer(UUnitActionContainer* NewContainer);
	void ReceiveActionContainer_Implementation(UUnitActionContainer* NewContainer);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Indicator")
	void UpdateShower(bool bIsIconPreview);
	void UpdateShower_Implementation(bool bIsIconPreview);


	UFUNCTION(BlueprintCallable, Category = "Pool")
	void InitializePool();
};
