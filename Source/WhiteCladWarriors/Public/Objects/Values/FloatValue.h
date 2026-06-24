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

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value", Meta = (ExposeOnSpawn = "true"))
	float CurrentValue;

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Value")
	bool GetIsEmpty() const;
	inline bool GetIsEmpty_Implementation() const { return CurrentValue <= 0.0f; }

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Value")
	float GetPercent() const;
	float GetPercent_Implementation() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Value")
	float SetPercent(float NewValue);
	float SetPercent_Implementation(float NewValue);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Value")
	float SetCurrentValue(float NewValue);
	float SetCurrentValue_Implementation(float NewValue);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Value")
	float SetMaxValue(float NewValue);
	float SetMaxValue_Implementation(float NewValue);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Value")
	float SetValue(float NewCurrentValue, float NewMaxValue);
	float SetValue_Implementation(float NewCurrentValue, float NewMaxValue);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Value")
	float AddValue(float Value);
	float AddValue_Implementation(float Value);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Value")
	float OnValueEmpty();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Value")
	float OnValueUnderflow(float AbsoluteValue);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Value")
	float BroadcastDirty();
	float BroadcastDirty_Implementation();
};