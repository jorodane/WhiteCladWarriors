// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Selectable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USelectable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class WHITECLADWARRIORS_API ISelectable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select", meta = (ExpandEnumAsExecs = "ReturnValue"))
	bool IsSelectable(class AOperator* Operator);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	void PlaySelectEffect(class AOperator* Operator, bool bIsSingleSelection);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	void Select(class AOperator* Operator, bool bIsSingleSelection);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	void Deselect();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	void MouseHoverBegin();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	void MouseHoverEnd();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	struct FSlateBrush GetSelectedIcon();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	FName GetSelectedName();
};
