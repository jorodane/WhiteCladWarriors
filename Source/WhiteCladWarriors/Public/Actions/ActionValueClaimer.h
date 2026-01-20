// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ActionValueClaimer.generated.h"

class UActionExecutor;
class UUnitActionComponent;

/**
 * 
 */
USTRUCT(BlueprintType)
struct FValueClaimer
{
	GENERATED_BODY()
};

UENUM(BlueprintType)
enum class EPositionGetterType : uint8
{
	SavedPosition, WorldPosition, SelfPosition, ActorPosition, CursorPosition
};

UENUM(BlueprintType)
enum class EPositionSpaceType : uint8 { Self, World };

USTRUCT(BlueprintType)
struct FPositionClaimer : public FValueClaimer
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value")
	FName PositionTag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value")
	EPositionGetterType PositionType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value")
	EPositionSpaceType AdditiveSpace;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value")
	FVector AdditivePosition;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value")
	bool bIsAdditive = false;

	FVector GetPosition(const UActionExecutor* Executor, const UUnitActionComponent* Component, const int ID) const;

	FVector GetAdditivePosition(const UUnitActionComponent* Component) const;
};

UENUM(BlueprintType)
enum EDirectionGetterType
{
	SavedDirection, Forward_World, Forward_Self, Forward_Actor, CursorDirection
};

USTRUCT(BlueprintType)
struct FDirectionClaimer : public FPositionClaimer
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value")
	FName DirectionTag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value")
	TEnumAsByte<EDirectionGetterType> DirectionType;

	FRotator GetRotator(const UActionExecutor* Executor, UUnitActionComponent* Component, const int ID) const;
	FVector GetDirection(const UActionExecutor* Executor, UUnitActionComponent* Component, const int ID) const;
};

USTRUCT(BlueprintType)
struct FActorClaimer : public FValueClaimer
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value")
	FName ActorTag;

	AActor* GetActor(const UActionExecutor* Executor, const UUnitActionComponent* Component, const int ID) const;
};

USTRUCT(BlueprintType)
struct FActorArrayClaimer : public FValueClaimer
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value")
	FName ActorArrayTag;

	TArray<AActor*> GetActorArray(const UActionExecutor* Executor, const UUnitActionComponent* Component, const int ID) const;
};