// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/Selectables/Components/UnitMainComponent.h"
#include "HeroMainComponent.generated.h"

class UUnitActionContainer;
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class WHITECLADWARRIORS_API UHeroMainComponent : public UUnitMainComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hero")
	FSlateBrush Portrait;

	UPROPERTY(BlueprintReadWrite, Category = "Action")
	TArray<UUnitActionContainer*> HeroActions;
};
