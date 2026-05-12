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
	bool CheckSelectable(class AOperator* Operator, bool bIsSingleSelection);
	virtual bool CheckSelectable_Implementation(class AOperator* Operator, bool bIsSingleSelection) { return false; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	void PlaySelectEffect(class AOperator* Operator, bool bIsSingleSelection);
	virtual void PlaySelectEffect_Implementation(class AOperator* Operator, bool bIsSingleSelection) {}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	void Select(class AOperator* Operator, bool bIsSingleSelection);
	virtual void Select_Implementation(class AOperator* Operator, bool bIsSingleSelection) {}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	void Deselect();
	virtual void Deselect_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	void MouseHoverBegin();
	virtual void MouseHoverBegin_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	void MouseHoverEnd();
	virtual void MouseHoverEnd_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	struct FSlateBrush GetSelectedIcon();
	virtual struct FSlateBrush GetSelectedIcon_Implementation() { return FSlateBrush(); }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	FText GetSelectedName();
	virtual FText GetSelectedName_Implementation() { return FText::GetEmpty(); }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	float GetSelectedorder();
	virtual float GetSelectedorder_Implementation() { return FLT_MAX; }
};
