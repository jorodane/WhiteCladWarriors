// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Generals/Structs/ActionStructures.h"
#include "InputPackage.generated.h"


class AActionBase;
class UActionNode;
class UActionSelectorNode;
class UActionExecutor;
class UUnitActionComponent;

UENUM(BlueprintType)
enum class EInputType : uint8
{
	Position, Direction, SingleTarget, MultiTarget,
};

USTRUCT(BlueprintType)
struct FSelectorInput
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	EInputType Type = EInputType::Position;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	FName Tag = NAME_None;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	UActionNode* OnInputAccepted = nullptr;
};

USTRUCT(BlueprintType)
struct FInputPackage
{
	GENERATED_BODY()

	static FInputPackage Input_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	FVector DragStartPosition = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	FVector MouseTerrainPosition = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<AActor> MouseHitActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<AActor> MouseClickActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TArray<AActor*> SelectedActors;
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
	TEnumAsByte<EMouseCursor::Type> TargetCursorType = EMouseCursor::Type::Default;

	void Clear()
	{
		TargetActionCursor.Clear();
		TargetComponentArray.SetNum(0);
		TargetDescription = FText::GetEmpty();
		TargetCursorType = EMouseCursor::Default;
	}
};