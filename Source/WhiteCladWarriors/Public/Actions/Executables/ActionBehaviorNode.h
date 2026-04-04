// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actions/Executables/ActionNode.h"
#include "Generals/Structs/ActionInputStructures.h"
#include "ActionBehaviorNode.generated.h"


class AActionIndicatorShowerBase;
class AUnitBase;
class UUnitActionComponent;
/**
 * 
 */
UCLASS()
class WHITECLADWARRIORS_API UActionBehaviorNode : public UActionNode
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, BlueprintImplementableEvent, Category = "Indicator")
	FIndicatorClaim GetIndicatorClaim(const FInputClaim& TargetInput);

	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Indicator")
	void UpdateIndicatorArray(const FInputClaim& TargetInput, const FInputPackage& InputPackage, const TArray<AActionIndicatorShowerBase*>& TargetShower, bool bIsIconPreview);
	void UpdateIndicatorArray_Implementation(const FInputClaim& TargetInput, const FInputPackage& InputPackage, const TArray<AActionIndicatorShowerBase*>& TargetShowers, bool bIsIconPreview);

	UFUNCTION(BlueprintPure, BlueprintImplementableEvent, Category = "Indicator")
	void UpdateIndicatorSingle(const FInputClaim& TargetInput, const FInputPackage& InputPackage, UUnitActionComponent* TargetComponent, AActionIndicatorShowerBase* TargetShower, AUnitBase* TargetUnit, int Index, bool bIsIconPreview);
};
