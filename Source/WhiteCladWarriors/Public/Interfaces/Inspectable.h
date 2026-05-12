// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Inspectable.generated.h"

class UObject;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInspectable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class WHITECLADWARRIORS_API IInspectable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pool")
	TArray<UObject*> GetInspectorTargets();
	virtual TArray<UObject*> GetInspectorTargets_Implementation() { return TArray<UObject*>(); }
};
