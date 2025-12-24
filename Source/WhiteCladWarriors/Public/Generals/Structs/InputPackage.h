// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputPackage.generated.h"


class UActionSelectorNode;
class UActionExecutor;
class UUnitActionComponent;

USTRUCT(BlueprintType)
struct FInputPackage
{
	GENERATED_BODY()

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
	TObjectPtr<UUnitActionComponent> TargetComponent;

	UPROPERTY(BlueprintReadWrite, Category = "Action")
	TObjectPtr<UActionSelectorNode> TargetNode;

	UPROPERTY(BlueprintReadWrite, Category = "Action")
	TObjectPtr<UActionExecutor> TargetExecutor;

	UPROPERTY(BlueprintReadWrite, Category = "Action")
	FText TargetDescription;

	UPROPERTY(BlueprintReadWrite, Category = "Action")
	TEnumAsByte<EMouseCursor::Type> TargetCursor;

	FInputClaim()
	{
		TargetNode = nullptr;
		TargetExecutor = nullptr;
	}
};