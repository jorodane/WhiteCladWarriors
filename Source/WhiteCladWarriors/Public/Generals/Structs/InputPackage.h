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

