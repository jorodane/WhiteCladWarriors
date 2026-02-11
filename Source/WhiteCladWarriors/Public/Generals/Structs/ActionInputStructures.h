// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Generals/Structs/ActionStructures.h"
#include "Generals/Structs/InputPackage.h"
#include "ActionInputStructures.generated.h"


class AActionBase;
class UActionNode;
class UActionSelectorNode;
class UActionExecutor;
class UUnitActionComponent;

USTRUCT(BlueprintType)
struct FInputClaim
{
	GENERATED_BODY()

	static FInputClaim Claim_None;

	UPROPERTY(BlueprintReadWrite, Category = "Action")
	FActionCursorFinder TargetActionCursor;

	UPROPERTY(BlueprintReadWrite, Category = "Action")
	TArray<UUnitActionComponent*> TargetComponentArray;

	UPROPERTY(BlueprintReadWrite, Category = "Action")
	TObjectPtr<UActionSelectorNode> TargetNode = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Action")
	FText TargetDescription;

	UPROPERTY(BlueprintReadWrite, Category = "Action")
	TEnumAsByte<EMouseCursor::Type> TargetCursorType = EMouseCursor::Type::Default;

	void Clear()
	{
		TargetActionCursor.Clear();
		TargetComponentArray.SetNum(0);
		TargetDescription = FText::GetEmpty();
		TargetCursorType = EMouseCursor::Default;
	}
};