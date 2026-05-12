// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ContainTagValue.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UContainTagValue : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class WHITECLADWARRIORS_API IContainTagValue
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SetValue")
	void SetTag(const FText& Value);
	virtual void SetTag_Implementation(const FText& Value) {}
};
