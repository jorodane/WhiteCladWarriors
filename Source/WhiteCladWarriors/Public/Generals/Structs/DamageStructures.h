// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageStructures.generated.h"

class UUnitMainComponent;

USTRUCT(BlueprintType)
struct FDamageInfo
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Damage")
	float DamageValue = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Damage")
	FVector DamageDirection = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Damage")
	AActor* DamageCauser = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Damage")
	UUnitMainComponent* DamageInstigator = nullptr;
};