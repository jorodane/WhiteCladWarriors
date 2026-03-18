// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Generals/Structs/ActionInputStructures.h"
#include "ActionIndicatorBase.generated.h"

class AOperator;
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
	TObjectPtr<UActionSelectorNode> CurrentNode;

	UPROPERTY(BlueprintReadOnly, Category = "Indicator")
	TArray<UUnitActionComponent*> CurrentComponents;

	TMap<EInputIndicatorType, TArray<AActionIndicatorShowerBase*>> ShowerPoolMap;
	TMap<UActionBehaviorNode*, TSet<AActionIndicatorShowerBase*>> ShowerActiveMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Indicator")
	TMap<EInputIndicatorType, TSubclassOf<AActionIndicatorShowerBase>> ShowerDefaultClassMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Indicator")
	int ShowerPoolDefaultCount = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Indicator")
	int ShowerPoolAdditionalCount = 4;

	UPROPERTY(BlueprintReadWrite, Category = "Indicator")
	bool bIsActivated = false;

public:
	void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Indicator")
	AActionIndicatorShowerBase* ShowerPop(EInputIndicatorType WantType);

	UFUNCTION(BlueprintCallable, Category = "Indicator")
	void ShowerPush(EInputIndicatorType WantType, AActionIndicatorShowerBase* WantShower);

	UFUNCTION(BlueprintCallable, Category = "Indicator")
	void ShowerPushFromSet(EInputIndicatorType WantType, AActionIndicatorShowerBase* WantShower, UActionBehaviorNode* TargetNode);

	UFUNCTION(BlueprintCallable, Category = "Indicator")
	void ShowerPrefareWithoutTemplate(EInputIndicatorType WantType, int Count);

	UFUNCTION(BlueprintCallable, Category = "Indicator")
	void ShowerPrefare(EInputIndicatorType WantType, TSubclassOf<AActionIndicatorShowerBase> WantClass, int Count);

	AActionIndicatorShowerBase* ShowerCreateAndPush(UWorld* WorldContext, TSubclassOf<AActionIndicatorShowerBase> Template, TArray<AActionIndicatorShowerBase*>& Pool);

	UFUNCTION(BlueprintCallable, Category = "Indicator")
	void ShowerClear();

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
