// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/Selectables/Units/UnitBase.h"
#include "HeroBase.generated.h"

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

};
