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

UENUM(BlueprintType)
enum class EInputIndicatorType : uint8
{
	Arrow, Circle, Quad, Bridge, Range, StaticMesh, SkeletalMesh
};

UENUM(BlueprintType)
enum class EInputMouseCursorType : uint8
{
	Default, Attack, Target, Selectable, Impossible, Enter, Exit, Up,Down,Left,Right, Grab, Work, Rotate
};

USTRUCT(BlueprintType)
struct FIndicatorClaim
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Indicator")
	EInputIndicatorType IndicatorType = EInputIndicatorType::Arrow;

	UPROPERTY(BlueprintReadWrite, Category = "Indicator")
	int Amount = 0;
};

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
	FText TargetMouseCursorDescription;

	UPROPERTY(BlueprintReadWrite, Category = "Action")
	EInputMouseCursorType TargetMouseCursorType = EInputMouseCursorType::Default;

	void Clear()
	{
		TargetNode = nullptr;
		TargetActionCursor.Clear();
		TargetComponentArray.SetNum(0);
		TargetMouseCursorDescription = TargetDescription = FText::GetEmpty();
		TargetMouseCursorType = EInputMouseCursorType::Default;
	}
};