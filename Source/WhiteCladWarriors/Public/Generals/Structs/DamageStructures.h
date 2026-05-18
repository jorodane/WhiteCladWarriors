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
	float DamageValue;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Damage")
	FVector DamageDirection;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Damage")
	AActor* DamageCauser;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Damage")
	UUnitMainComponent* DamageInstigator;
};