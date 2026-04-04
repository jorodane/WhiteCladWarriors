// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UnitActionContainer.generated.h"

class AActionBase;
class UUnitActionComponent;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class WHITECLADWARRIORS_API UUnitActionContainer : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Category = "Action")
	TObjectPtr<AActionBase> CurrentAction;

	UPROPERTY(BlueprintReadOnly, Category = "Action")
	TArray<UUnitActionComponent*> CurrentComponents;

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	AActionBase* SetAction(AActionBase* NewAction);
	AActionBase* SetAction_Implementation(AActionBase* NewAction) {return CurrentAction = NewAction;};

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	TArray<UUnitActionComponent*> SetComponents(const TArray<UUnitActionComponent*>& NewComponents);
	TArray<UUnitActionComponent*> SetComponents_Implementation(const TArray<UUnitActionComponent*>& NewComponents) { return CurrentComponents = NewComponents;};

};
