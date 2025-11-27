// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ContainFillColorPaint.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UContainFillColorPaint : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class WHITECLADWARRIORS_API IContainFillColorPaint
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SetColor")
	void SetFillColor(FLinearColor WantColor);
};
