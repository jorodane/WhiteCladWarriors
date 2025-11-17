// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "IngameController.generated.h"

/**
 * 
 */
class AUnitBase;
class AOperator;

UCLASS()
class WHITECLADWARRIORS_API AIngameController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Player")
	TObjectPtr<AOperator> Operator;

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	TObjectPtr<AUnitBase> Hero;

	
};
