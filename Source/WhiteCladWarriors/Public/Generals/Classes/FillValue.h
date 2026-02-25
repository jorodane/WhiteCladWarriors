// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FillValue.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnValueChanged, float, CurrentValue, float, MaxValue, float, Ratio);

UCLASS()
class UFillValue : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "FillValue")
	FOnValueChanged OnValueChanged;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FillValue", Meta = (ExposeOnSpawn = "true"))
	float CurrentValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FillValue", Meta = (ExposeOnSpawn = "true"))
	float MaxValue;

public:
	UFUNCTION(BlueprintPure, Category = "FillValue")
	float GetPercent() const;

	UFUNCTION(BlueprintCallable, Category = "FillValue")
	float SetPercent(float NewValue);

	UFUNCTION(BlueprintCallable, Category = "FillValue")
	float SetCurrentValue(float NewValue);

	UFUNCTION(BlueprintCallable, Category = "FillValue")
	float SetMaxValue(float NewValue);

	UFUNCTION(BlueprintCallable, Category = "FillValue")
	void SetValue(float NewCurrentValue, float NewMaxValue);

	UFUNCTION(BlueprintCallable, Category = "FillValue")
	float BroadcastDirty();
};