// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/HitResult.h"
#include "Engine/DecalActor.h"
#include "AreaSelector.generated.h"

class AOperator;

UCLASS()
class WHITECLADWARRIORS_API AAreaSelector : public ADecalActor
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Select")
	TArray<FHitResult> HitTraceInArea();
	virtual TArray<FHitResult> HitTraceInArea_Implementation();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Select")
	TArray<AActor*> GetActorsInArea(AOperator* Operator, bool& bIsAllSame, bool& bIsSingleSelected);
	virtual TArray<AActor*> GetActorsInArea_Implementation(AOperator* Operator, bool& bIsAllSame, bool& bIsSingleSelected);
};
