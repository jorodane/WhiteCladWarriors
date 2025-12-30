// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Generals/Structs/InputPackage.h"
#include "Objects/Selectables/Components/UnitComponentBase.h"
#include "UnitActionComponent.generated.h"


class AActionBase;
class AUnitBase;

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

	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Variable")
	FVector GetLocation();
	FVector GetLocation_Implementation();

	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Variable")
	FVector GetDirection(FVector Destination, bool bIsIgnoreZ = false);
	FVector GetDirection_Implementation(FVector Destination, bool bIsIgnoreZ = false);

	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Variable")
	AUnitBase* GetOwnerUnit();
	AUnitBase* GetOwnerUnit_Implementation();

	UFUNCTION(BlueprintPure, Category = "Action")
	bool GetMainActionCancelable();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	bool TrySetMainAction(UActionExecutor* Executor, bool bIsCancelable);
	bool TrySetMainAction_Implementation(UActionExecutor* Executor, bool bIsCancelable);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void EndMainAction(UActionExecutor* Executor);
	void EndMainAction_Implementation(UActionExecutor* Executor);
};
