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
	virtual bool GetMainActionCancelable() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	bool TrySetMainAction(const FActionCursorFinder& WantCursor, UActionNode* TargetNode);
	virtual bool TrySetMainAction_Implementation(const FActionCursorFinder& WantCursor, UActionNode* TargetNode);

	UFUNCTION(BlueprintCallable, Category = "Action")
	virtual void EndMainAction(int64 ExecutorID, UUnitActionComponent* OldComponent);

	UFUNCTION(BlueprintImplementableEvent, Category = "Action")
	void OnEndMainAction(int64 ExecutorID);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	void OnInputStart(const FInputClaim& StartedInput);
	virtual void OnInputStart_Implementation(const FInputClaim& StartedInput);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	void OnInputEnd(const FInputClaim& EndedInput);
	virtual void OnInputEnd_Implementation(const FInputClaim& EndedInput);
};
