// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/Values/ValueObject.h"
#include "FloatValue.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurrentValueChanged, float, Value);

UCLASS(Blueprintable, BlueprintType)
class UFloatValue : public UValueObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Value")
	FOnCurrentValueChanged OnCurrentValueChanged;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value", Meta = (ExposeOnSpawn = "true"))
	float CurrentValue;

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Value")
	bool GetIsEmpty() const;
	inline bool GetIsEmpty_Implementation() const { return CurrentValue <= 0.0f; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Value")
	float GetValue();
	virtual float GetValue_Implementation() { return CurrentValue; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Value")
	float SetValue(float NewValue);
	virtual float SetValue_Implementation(float NewValue);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Value")
	float AddValue(float Value);
	virtual float AddValue_Implementation(float Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Value")
	float BroadcastDirty();
	virtual float BroadcastDirty_Implementation();
};