// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/Selectables/Components/UnitComponentBase.h"
#include "FillableValueComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnValueChanged, float, CurrentValue, float, MaxValue, float, Ratio);

UCLASS()
class UFillableValueComponent : public UUnitComponentBase
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
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "FillValue")
	float GetPercent() const;
	float GetPercent_Implementation() const;

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
	float BroadcastDirty();
	float BroadcastDirty_Implementation();
};