// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GeneralEnumerators.generated.h"

UENUM(BlueprintType)
enum class EValidPinType : uint8
{
	IsNotValid = 0,
	IsValid = 1,
};