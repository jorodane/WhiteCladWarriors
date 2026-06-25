// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/Values/FloatValue.h"
#include "FillableValue.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnFillableValueChanged, float, CurrentValue, float, MaxValue, float, Ratio);

UCLASS(Blueprintable, BlueprintType)
class UFillableValue : public UFloatValue
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "FillValue")
	FOnFillableValueChanged OnFillableValueChanged;

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FillValue", Meta = (ExposeOnSpawn = "true"))
	float MaxValue;

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "FillValue")
	bool GetIsFull() const;
	inline bool GetIsFull_Implementation() const { return CurrentValue >= MaxValue; }

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "FillValue")
	float GetPercent() const;
	float GetPercent_Implementation() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "FillValue")
	float GetEmptySpace() const;
	inline float GetEmptySpace_Implementation() const { return MaxValue - CurrentValue; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "FillValue")
	float SetPercent(float NewValue);
	float SetPercent_Implementation(float NewValue);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "FillValue")
	float SetMaxValue(float NewValue);
	float SetMaxValue_Implementation(float NewValue);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "FillValue")
	float SetFillValue(float NewCurrentValue, float NewMaxValue);
	float SetFillValue_Implementation(float NewCurrentValue, float NewMaxValue);

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "FillValue")
	float OnValueFull();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Value")
	float OnValueEmpty();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "FillValue")
	float OnValueOverflow(float AbsoluteValue);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Value")
	float OnValueUnderflow(float AbsoluteValue);

public:
	virtual float SetValue_Implementation(float NewValue) override;
	virtual float AddValue_Implementation(float Value) override;
	virtual float BroadcastDirty_Implementation() override;
};