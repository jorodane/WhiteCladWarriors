// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ActionValueClaimer.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FValueClaimer
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value")
	FName ClaimTag;
};

USTRUCT(BlueprintType)
struct FPositionClaimer : public FValueClaimer
{
	GENERATED_BODY()

};

USTRUCT(BlueprintType)
struct FDirectionClaimer : public FPositionClaimer
{
	GENERATED_BODY()

};

USTRUCT(BlueprintType)
struct FActorClaimer : public FValueClaimer
{
	GENERATED_BODY()

};

USTRUCT(BlueprintType)
struct FActorArrayClaimer : public FValueClaimer
{
	GENERATED_BODY()

};