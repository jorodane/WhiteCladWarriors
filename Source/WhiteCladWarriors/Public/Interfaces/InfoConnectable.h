// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InfoConnectable.generated.h"

class UWidget;
class AOperator;

UENUM(BlueprintType)
enum class EInfoWidgetType : uint8
{
	Minimal, Hover, Detail, Inspector
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInfoConnectable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class WHITECLADWARRIORS_API IInfoConnectable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericWidget")
	TArray<UOrderedGenericWidgetClaim*> GetInfoWidget(EInfoWidgetType WantType, AOperator* Operator) const;
	virtual TArray<UOrderedGenericWidgetClaim*> GetInfoWidget_Implementation(EInfoWidgetType WantType, AOperator* Operator) const { return TArray<UOrderedGenericWidgetClaim*>(); }
};
