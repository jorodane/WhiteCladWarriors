// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Generals/Structs/ActionInputStructures.h"
#include "ActionIndicatorShowerBase.generated.h"

UCLASS()
class WHITECLADWARRIORS_API AActionIndicatorShowerBase : public AActor
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintPure, Category = "Indicator")
	EInputIndicatorType GetIndicatorType();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Pool")
	void OnPoolEnqueue();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Pool")
	void OnPoolDequeue();
};
