// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Generals/Structs/InputPackage.h"
#include "Generals/Structs/ActionInputStructures.h"
#include "Objects/Selectables/Components/UnitComponentBase.h"
#include "UnitActionComponent.generated.h"


class AActionBase;
class UUnitMainComponent;

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
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Spawn", meta = (DeterminesOutputType = "TemplateClass"))
	AActor* SpawnActor(TSubclassOf<AActor> TemplateClass);
	AActor* SpawnActor_Implementation(TSubclassOf<AActor> TemplateClass);

	UFUNCTION(BlueprintPure, Category = "Action")
	bool GetMainActionCancelable();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	bool TrySetMainAction(const FActionCursorFinder& WantCursor, bool bIsCancelable, bool bIsStopMovement);
	bool TrySetMainAction_Implementation(const FActionCursorFinder& WantCursor, bool bIsCancelable, bool bIsStopMovement);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void EndMainAction(UActionExecutor* Executor, bool bIsStopMovement);

	UFUNCTION(BlueprintImplementableEvent, Category = "Action")
	void OnEndMainAction(UActionExecutor* Executor, bool bIsStopMovement);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	void OnInputStart(const FInputClaim& StartedInput);
	void OnInputStart_Implementation(const FInputClaim& StartedInput);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	void OnInputEnd(const FInputClaim& EndedInput);
	void OnInputEnd_Implementation(const FInputClaim& EndedInput);
};
