// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ContainToggleValue.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FToggleDelegate, bool, Value);

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UContainToggleValue : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class WHITECLADWARRIORS_API IContainToggleValue
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SetValue")
	void SetToggle(bool InitialValue, const FToggleDelegate& OnToggled);
	virtual void SetToggle_Implementation(bool InitialValue, const FToggleDelegate& OnToggled) {}

};
