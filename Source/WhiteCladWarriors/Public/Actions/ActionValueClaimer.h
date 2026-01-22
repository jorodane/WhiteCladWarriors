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
struct FVectorGetter
{
	GENERATED_BODY()

	FVector Value;

	virtual FVector GetValue() const { return Value; }
};

USTRUCT(BlueprintType)
struct FVectorGetter_Simple : public FVectorGetter
{
	GENERATED_BODY()


	FVectorGetter_Simple(){}
	FVectorGetter_Simple(double Value)
	{

	}

	FVectorGetter_Simple(FVector Value)
	{

	}

	virtual FVector GetValue() const override { return Value; }
};

UCLASS()
class UValueGetter : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Value")
	static FVectorGetter MakeSimpleVector(FVector Value) { return FVectorGetter_Simple(Value); }
};

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

USTRUCT(BlueprintType)
struct FDirectionClaimer : public FValueClaimer
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value")
	FPositionClaimer From;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value")
	FPositionClaimer To;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value")
	FName DirectionTag;

	FVector GetPosition(const UActionExecutor* Executor, const UUnitActionComponent* Component, const int ID) const;
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