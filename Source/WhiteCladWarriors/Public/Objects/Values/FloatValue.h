// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/Values/ValueObject.h"
#include "FloatValue.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFloatValueChanged, float, NewValue);

UCLASS(Blueprintable, BlueprintType)
class UFloatValue : public UValueObject
{
	GENERATED_BODY()

public:
	//UPROPERTY(BlueprintAssignable, Category = "FillValue")
	//FOnFloatValueChanged OnFloatValueChanged;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FillValue", Meta = (ExposeOnSpawn = "true"))
	float CurrentValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FillValue", Meta = (ExposeOnSpawn = "true"))
	float MaxValue;

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "FillValue")
	bool GetIsEmpty() const;
	inline bool GetIsEmpty_Implementation() const { return CurrentValue <= 0.0f; }

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "FillValue")
	bool GetIsFull() const;
	inline bool GetIsFull_Implementation() const { return CurrentValue >= MaxValue; }

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "FillValue")
	float GetPercent() const;
	float GetPercent_Implementation() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "FillValue")
	float GetFillableValue() const;
	inline float GetFillableValue_Implementation() const { return MaxValue - CurrentValue; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "FillValue")
	float SetPercent(float NewValue);
	float SetPercent_Implementation(float NewValue);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "FillValue")
	float SetCurrentValue(float NewValue);
	float SetCurrentValue_Implementation(float NewValue);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "FillValue")
	float SetMaxValue(float NewValue);
	float SetMaxValue_Implementation(float NewValue);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "FillValue")
	float SetValue(float NewCurrentValue, float NewMaxValue);
	float SetValue_Implementation(float NewCurrentValue, float NewMaxValue);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "FillValue")
	float AddValue(float Value);
	float AddValue_Implementation(float Value);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "FillValue")
	float OnValueFull();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "FillValue")
	float OnValueEmpty();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "FillValue")
	float OnValueOverflow(float AbsoluteValue);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "FillValue")
	float OnValueUnderflow(float AbsoluteValue);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "FillValue")
	float BroadcastDirty();
	float BroadcastDirty_Implementation();
};