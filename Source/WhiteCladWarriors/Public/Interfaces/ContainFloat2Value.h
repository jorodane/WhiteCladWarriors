// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ContainFloat2Value.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UContainFloat2Value : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class WHITECLADWARRIORS_API IContainFloat2Value
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SetValue")
	void SetFloat2(float Value, float Value2);
};
