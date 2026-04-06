// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/Selectables/Components/UnitMainComponent.h"
#include "HeroMainComponent.generated.h"

class UUnitActionContainer;
/**
 * 
 */
UCLASS()
class WHITECLADWARRIORS_API AHeroBase : public AUnitBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hero")
	FText ClassName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hero")
	FSlateBrush Portrait;

	UPROPERTY(BlueprintReadWrite, Category = "Action")
	TArray<UUnitActionContainer*> HeroActions;
};
