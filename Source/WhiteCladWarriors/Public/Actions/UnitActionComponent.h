// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Generals/Structs/InputPackage.h"
#include "Objects/Selectables/Components/UnitComponentBase.h"
#include "UnitActionComponent.generated.h"

class AActionBase;
/**
 * 
 */
UCLASS()
class WHITECLADWARRIORS_API UUnitActionComponent : public UUnitComponentBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	TArray<FName> ActionList;
};
