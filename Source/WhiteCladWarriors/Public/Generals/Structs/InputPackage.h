// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	EInputType Type;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	FName Tag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	UActionNode* OnInputAccepted;
};

USTRUCT(BlueprintType)
struct FInputPackage
{
	GENERATED_BODY()

	static FInputPackage Input_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	FVector DragStartPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	FVector MouseTerrainPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<AActor> MouseHitActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<AActor> MouseClickActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TArray<AActor*> SelectedActors;
};

USTRUCT(BlueprintType)
struct FInputClaim
{
	GENERATED_BODY()

	static FInputClaim Claim_None;

	UPROPERTY(BlueprintReadWrite, Category = "Action")
	TArray<UUnitActionComponent*> TargetComponentArray;

	UPROPERTY(BlueprintReadWrite, Category = "Action")
	TObjectPtr<AActionBase> TargetAction;

	UPROPERTY(BlueprintReadWrite, Category = "Action")
	TObjectPtr<UActionSelectorNode> TargetNode;

	UPROPERTY(BlueprintReadWrite, Category = "Action")
	TObjectPtr<UActionExecutor> TargetExecutor;

	UPROPERTY(BlueprintReadWrite, Category = "Action")
	FText TargetDescription;

	UPROPERTY(BlueprintReadWrite, Category = "Action")
	TEnumAsByte<EMouseCursor::Type> TargetCursor;

	UPROPERTY(BlueprintReadWrite, Category = "Action")
	int ID;

	void Clear()
	{
		TargetComponentArray.SetNum(0);
		TargetAction = nullptr;
		TargetNode = nullptr;
		TargetExecutor = nullptr;
		TargetDescription = FText::GetEmpty();
		TargetCursor = EMouseCursor::Default;
		ID = 0;
	}
};