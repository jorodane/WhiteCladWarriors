// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Kismet/KismetMathLibrary.h"
#include "ActionValueClaimer.generated.h"

class UActionExecutor;
class UUnitActionComponent;

/**
 * 
 */
UCLASS(Abstract, BlueprintType)
class UVectorGetter : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value", Meta = (ExposeOnSpawn = "true"))
	FVector Value;

	virtual FVector GetValue() const { return Value; }
};

UCLASS(BlueprintType)
class UVectorGetter_Simple : public UVectorGetter
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Value")
	void SetValueSingle(double WantValue) {Value.X = Value.Y = Value.Z = WantValue;}

	UFUNCTION(BlueprintCallable, Category = "Value")
	void SetValue(FVector WantValue) {Value = WantValue;}

	virtual FVector GetValue() const override { return Value; }
};

UCLASS(BlueprintType)
class UVectorGetter_Random : public UVectorGetter
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value", Meta = (ExposeOnSpawn = "true"))
	FVector MaxValue;

	UFUNCTION(BlueprintCallable, Category = "Value")
	void SetValueSingle(double Min, double Max) { Value.X = Value.Y = Value.Z = Min; MaxValue.X = MaxValue.Y = MaxValue.Z = Max; }
	UFUNCTION(BlueprintCallable, Category = "Value")
	void SetValue(FVector Min, FVector Max) { Value = Min; MaxValue = Max; }

	virtual FVector GetValue() const override 
	{
		FVector Result = FVector::ZeroVector;
		Result.X = FMath::RandRange(Value.X, MaxValue.X);
		Result.Y = FMath::RandRange(Value.Y, MaxValue.Y);
		Result.Z = FMath::RandRange(Value.Z, MaxValue.Z); 
		return Result;
	}
};

UCLASS(BlueprintType)
class UVectorGetter_RandomSphere : public UVectorGetter
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value", Meta = (ExposeOnSpawn = "true"))
	FVector Bound;

	UFUNCTION(BlueprintCallable, Category = "Value")
	void SetValue(FVector Center, FVector MaxBound) { Value = Center; Bound = MaxBound; }

	virtual FVector GetValue() const override
	{
		FVector Dir = FMath::VRand();
		const float r = FMath::Pow(FMath::FRand(), 1.f / 3.f);

		FVector Result = Dir * r;
		Result *= Bound;
		Result += Value;
		return Result;
	}
};

UCLASS(BlueprintType)
class UValueGetterLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Value", Meta = (DefaultToSelf = "Owner"))
	static UVectorGetter* MakeSimpleVector(UObject* Owner, FVector Value) 
	{ 
		UVectorGetter_Simple* Result = NewObject<UVectorGetter_Simple>(Owner);
		if (Result) Result->SetValue(Value);
		return Result;
	}

	UFUNCTION(BlueprintPure, Category = "Value", Meta = (DefaultToSelf = "Owner"))
	static UVectorGetter* MakeRandomVector(UObject* Owner, FVector Min, FVector Max)
	{
		UVectorGetter_Random* Result = NewObject<UVectorGetter_Random>(Owner);
		if (Result) Result->SetValue(Min, Max);
		return Result;
	}

	UFUNCTION(BlueprintPure, Category = "Value", Meta = (DefaultToSelf = "Owner"))
	static UVectorGetter* MakeRandomSphereVector(UObject* Owner, FVector Center, FVector Bound)
	{
		UVectorGetter_Random* Result = NewObject<UVectorGetter_Random>(Owner);
		if (Result) Result->SetValue(Center, Bound);
		return Result;
	}
};















UCLASS(BlueprintType)
class UValueClaimer : public UObject
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

UCLASS(BlueprintType)
class UPositionClaimer : public UValueClaimer
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value")
	FName PositionTag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value")
	EPositionGetterType PositionType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value")
	EPositionSpaceType AdditiveSpace;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value")
	TObjectPtr<UVectorGetter> AdditivePosition;

	UFUNCTION(BlueprintCallable, Category = "Value")
	void Set(FName WantPositionTag, EPositionGetterType WantPositionType, EPositionSpaceType WantAdditiveSpace, UVectorGetter* WantAdditivePosition)
	{
		PositionTag = WantPositionTag;
		PositionType = WantPositionType;
		AdditiveSpace = WantAdditiveSpace;
		AdditivePosition = WantAdditivePosition;
	}

	FVector GetPosition(const UActionExecutor* Executor, const UUnitActionComponent* Component, const int ID) const;

	FVector GetAdditivePosition(const UUnitActionComponent* Component) const;
};

UCLASS(BlueprintType)
class UDirectionClaimer : public UValueClaimer
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value")
	TObjectPtr<UPositionClaimer> From;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value")
	TObjectPtr<UPositionClaimer> To;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value")
	FName DirectionTag;

	UFUNCTION(BlueprintCallable, Category = "Value")
	void Set(UPositionClaimer* WantFrom, UPositionClaimer* WantTo, FName WantTag)
	{
		From = WantFrom;
		To = WantTo;
		DirectionTag = WantTag;
	}
	FVector GetPosition(const UActionExecutor* Executor, const UUnitActionComponent* Component, const int ID) const;
	FVector GetDirection(const UActionExecutor* Executor, UUnitActionComponent* Component, const int ID) const;
};

UCLASS(BlueprintType)
class UActorClaimer : public UValueClaimer
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value")
	FName ActorTag;

	UFUNCTION(BlueprintCallable, Category = "Value")
	void Set(FName WantTag)
	{
		ActorTag = WantTag;
	}

	AActor* GetActor(const UActionExecutor* Executor, const UUnitActionComponent* Component, const int ID) const;
};

UCLASS(BlueprintType)
class UActorArrayClaimer : public UValueClaimer
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value")
	FName ActorArrayTag;

	UFUNCTION(BlueprintCallable, Category = "Value")
	void Set(FName WantTag)
	{
		ActorArrayTag = WantTag;
	}

	TArray<AActor*> GetActorArray(const UActionExecutor* Executor, const UUnitActionComponent* Component, const int ID) const;
};

UCLASS(BlueprintType)
class UValueClaimerLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Value", Meta = (DefaultToSelf = "Owner"))
	static UPositionClaimer* MakePositionClaimer(UObject* Owner, FName WantPositionTag, EPositionGetterType WantPositionType, EPositionSpaceType WantAdditiveSpace, UVectorGetter* WantAdditivePosition)
	{
		UPositionClaimer* Result = NewObject<UPositionClaimer>(Owner);
		if (Result) Result->Set(WantPositionTag,WantPositionType,WantAdditiveSpace,WantAdditivePosition);
		return Result;
	}

	UFUNCTION(BlueprintPure, Category = "Value", Meta = (DefaultToSelf = "Owner"))
	static UDirectionClaimer* MakeDirectionClaimer(UObject* Owner, UPositionClaimer* WantFrom, UPositionClaimer* WantTo, FName WantTag)
	{
		UDirectionClaimer* Result = NewObject<UDirectionClaimer>(Owner);
		if (Result) Result->Set(WantFrom, WantTo, WantTag);
		return Result;
	}


	UFUNCTION(BlueprintPure, Category = "Value", Meta = (DefaultToSelf = "Owner"))
	static UActorClaimer* MakeActorClaimer(UObject* Owner, FName WantTag)
	{
		UActorClaimer* Result = NewObject<UActorClaimer>(Owner);
		if (Result) Result->Set(WantTag);
		return Result;
	}

	UFUNCTION(BlueprintPure, Category = "Value", Meta = (DefaultToSelf = "Owner"))
	static UActorArrayClaimer* MakeActorArrayClaimer(UObject* Owner, FName WantTag)
	{
		UActorArrayClaimer* Result = NewObject<UActorArrayClaimer>(Owner);
		if (Result) Result->Set(WantTag);
		return Result;
	}
};