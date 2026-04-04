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
	TObjectPtr<AActionBase> CurrentAction;
	TArray<UUnitActionComponent*> CurrentComponents;

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	AActionBase* SetCurrentAction(AActionBase* NewAction);
	AActionBase* SetCurrentAction_Implementation(AActionBase* NewAction) {return CurrentAction = NewAction;};


	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Action")
	AActionBase* GetCurrentAction();
	AActionBase* GetCurrentAction_Implementation() { return CurrentAction; };

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	TArray<UUnitActionComponent*> SetCurrentComponents(const TArray<UUnitActionComponent*>& NewComponents);
	TArray<UUnitActionComponent*> SetCurrentComponents_Implementation(const TArray<UUnitActionComponent*>& NewComponents) { return CurrentComponents = NewComponents;};


	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Action")
	TArray<UUnitActionComponent*> GetCurrentComponents();
	TArray<UUnitActionComponent*> GetCurrentComponents_Implementation() { return CurrentComponents; };

};
