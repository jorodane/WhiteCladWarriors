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

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Spawn", meta = (DeterminesOutputType = "TemplateClass"))
	AActor* SpawnActor(TSubclassOf<AActor> TemplateClass);
	AActor* SpawnActor_Implementation(TSubclassOf<AActor> TemplateClass);

	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Location")
	FVector GetLocation();
	FVector GetLocation_Implementation();

	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Location")
	FVector GetDirection(FVector Destination, bool bIsIgnoreZ = false);
	FVector GetDirection_Implementation(FVector Destination, bool bIsIgnoreZ = false);
};
