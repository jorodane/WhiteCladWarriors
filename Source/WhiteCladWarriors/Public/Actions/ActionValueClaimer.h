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
class UFloatGetter : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value", Meta = (ExposeOnSpawn = "true"))
	float Value;

	virtual float GetValue() const { return Value; }
};

UCLASS(BlueprintType)
class UFloatGetter_Simple : public UFloatGetter
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Value")
	void SetValueSingle(float WantValue) { Value = WantValue; }

	UFUNCTION(BlueprintCallable, Category = "Value")
	void SetValue(float WantValue) { Value = WantValue; }

	virtual float GetValue() const override { return Value; }
};

UCLASS(BlueprintType)
class UFloatGetter_Random : public UFloatGetter
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value", Meta = (ExposeOnSpawn = "true"))
	float MaxValue;

	UFUNCTION(BlueprintCallable, Category = "Value")
	void SetValueSingle(double Min, double Max) { Value = Min; MaxValue = Max; }
	UFUNCTION(BlueprintCallable, Category = "Value")
	void SetValue(float Min, float Max) { Value = Min; MaxValue = Max; }

	virtual float GetValue() const override { return FMath::RandRange(Value, MaxValue); }
};

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

	static TObjectPtr<UVectorGetter_Simple> ForwardVector;
	static TObjectPtr<UVectorGetter_Simple> BackwardVector;
	static TObjectPtr<UVectorGetter_Simple> RightVector;
	static TObjectPtr<UVectorGetter_Simple> LeftVector;
	static TObjectPtr<UVectorGetter_Simple> UpVector;
	static TObjectPtr<UVectorGetter_Simple> DownVector;

public:
	UValueGetterLibrary();

	UFUNCTION(BlueprintPure, Category = "Value", Meta = (DefaultToSelf = "Owner"))
	static UFloatGetter* MakeSimpleFloat(UObject* Owner, float Value)
	{
		UFloatGetter_Simple* Result = NewObject<UFloatGetter_Simple>(Owner);
		if (Result) Result->SetValue(Value);
		return Result;
	}

	UFUNCTION(BlueprintPure, Category = "Value", Meta = (DefaultToSelf = "Owner"))
	static UFloatGetter* MakeRandomFloat(UObject* Owner, float Min, float Max)
	{
		UFloatGetter_Random* Result = NewObject<UFloatGetter_Random>(Owner);
		if (Result) Result->SetValue(Min, Max);
		return Result;
	}

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

	UFUNCTION(BlueprintPure, Category = "Value")
	static UVectorGetter* GetSimpleForwardVector() { return ForwardVector;}
	UFUNCTION(BlueprintPure, Category = "Value")
	static UVectorGetter* GetSimpleBackwardVector() { return BackwardVector;}
	UFUNCTION(BlueprintPure, Category = "Value")
	static UVectorGetter* GetSimpleRightVector() { return RightVector;}
	UFUNCTION(BlueprintPure, Category = "Value")
	static UVectorGetter* GetSimpleLeftVector() { return LeftVector;}
	UFUNCTION(BlueprintPure, Category = "Value")
	static UVectorGetter* GetSimpleUpVector() { return UpVector;}
	UFUNCTION(BlueprintPure, Category = "Value")
	static UVectorGetter* GetSimpleDownVector() { return DownVector;}
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

UCLASS(Abstract, BlueprintType)
class UDirectionClaimer : public UValueClaimer
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value")
	TObjectPtr<UPositionClaimer> From;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value")
	TObjectPtr<UFloatGetter> AngleShift;

	virtual FVector GetPosition(const UPositionClaimer* Claimer, const UActionExecutor* Executor, const UUnitActionComponent* Component, const int ID) const;
	virtual FVector GetOriginDirection(const UActionExecutor* Executor, UUnitActionComponent* Component, const int ID) const {return FVector::ZeroVector;};
	inline virtual FVector GetStartPosition(const UActionExecutor* Executor, UUnitActionComponent* Component, const int ID) const { return GetPosition(From, Executor, Component, ID); }
	inline virtual FVector GetEndPosition(const UActionExecutor* Executor, UUnitActionComponent* Component, const int ID) const { return GetStartPosition(Executor,Component,ID) + GetOriginDirection(Executor,Component,ID); }
	virtual FVector GetDirection(const UActionExecutor* Executor, UUnitActionComponent* Component, const int ID) const;
};

UCLASS(BlueprintType)
class UDirectionClaimer_ToPosition : public UDirectionClaimer
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value")
	TObjectPtr<UPositionClaimer> To;


	UFUNCTION(BlueprintCallable, Category = "Value")
	void Set(UPositionClaimer* WantFrom, UPositionClaimer* WantTo, UFloatGetter* WantAngleShift)
	{
		From = WantFrom;
		To = WantTo;
		AngleShift = WantAngleShift;
	}
	virtual FVector GetOriginDirection(const UActionExecutor* Executor, UUnitActionComponent* Component, const int ID) const override ;
	inline virtual FVector GetEndPosition(const UActionExecutor* Executor, UUnitActionComponent* Component, const int ID) const override  { return GetPosition(To, Executor, Component, ID); }
};

UCLASS(BlueprintType)
class UDirectionClaimer_SimpleDirection : public UDirectionClaimer
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value")
	EPositionSpaceType DirectionSpace;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value")
	TObjectPtr<UVectorGetter> DirectionTo;

	UFUNCTION(BlueprintCallable, Category = "Value")
	void Set(UPositionClaimer* WantFrom, EPositionSpaceType WantSpace, UVectorGetter* WantDirection, UFloatGetter* WantAngleShift)
	{
		From = WantFrom;
		DirectionSpace = WantSpace;
		DirectionTo = WantDirection;
		AngleShift = WantAngleShift;
	}
	virtual FVector GetOriginDirection(const UActionExecutor* Executor, UUnitActionComponent* Component, const int ID) const override 
	{
		if(IsValid(DirectionTo)) return DirectionTo->GetValue();
		else return FVector::ZeroVector;
	}
};

UCLASS(BlueprintType)
class UDirectionClaimer_SavedDirection : public UDirectionClaimer
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value")
	FName DirectionTag;

	UFUNCTION(BlueprintCallable, Category = "Value")
	void Set(UPositionClaimer* WantFrom, FName WantTag, UFloatGetter* WantAngleShift)
	{
		From = WantFrom;
		DirectionTag = WantTag;
		AngleShift = WantAngleShift;
	}
	virtual FVector GetOriginDirection(const UActionExecutor* Executor, UUnitActionComponent* Component, const int ID) const override;
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

UENUM(BlueprintType)
enum class EActorPositionResult : uint8
{
	Position, Actor
};

UCLASS(BlueprintType)
class UActorOrPositionClaimer : public UValueClaimer
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value")
	TObjectPtr<UPositionClaimer> PositionClaimer;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value")
	TObjectPtr<UActorClaimer> ActorClaimer;

	UFUNCTION(BlueprintCallable, Category = "Value")
	void Set(UPositionClaimer* WantPosition, UActorClaimer* WantActor)
	{
		PositionClaimer = WantPosition;
		ActorClaimer = WantActor;
	}

	UFUNCTION(BlueprintCallable, Category = "Value")
	inline FVector GetPosition(const UActionExecutor* Executor, const UUnitActionComponent* Component, const int ID) const { if (IsValid(PositionClaimer)) return PositionClaimer->GetPosition(Executor, Component, ID); return FVector::ZeroVector; };

	UFUNCTION(BlueprintCallable, Category = "Value")
	inline AActor* GetActor(const UActionExecutor* Executor, const UUnitActionComponent* Component, const int ID) const { if (IsValid(ActorClaimer)) return ActorClaimer->GetActor(Executor, Component, ID); return nullptr; };

	UFUNCTION(BlueprintCallable, Category = "Value", meta = (ExpandEnumAsExecs = "ReturnValue"))
	EActorPositionResult TryGetActorOrPosition(const UActionExecutor* Executor, const UUnitActionComponent* Component, const int ID, FVector& ResultPosition, AActor*& ResultAsActor) const
	{
		ResultAsActor = GetActor(Executor, Component, ID);
		if (ResultAsActor)
		{
			ResultPosition = ResultAsActor->GetActorLocation();
			return EActorPositionResult::Actor;
		}
		else
		{
			ResultPosition = GetPosition(Executor, Component, ID);
			return EActorPositionResult::Position;
		}
	}
};

UCLASS(BlueprintType)
class UValueClaimerLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	static TObjectPtr<UPositionClaimer> SelfPosition;
	static TObjectPtr<UPositionClaimer> SelfForwardPosition;
	static TObjectPtr<UPositionClaimer> SelfBackwardPosition;
	static TObjectPtr<UPositionClaimer> SelfRightPosition;
	static TObjectPtr<UPositionClaimer> SelfLeftPosition;
	static TObjectPtr<UPositionClaimer> SelfUpPosition;
	static TObjectPtr<UPositionClaimer> SelfDownPosition;

public:
	UValueClaimerLibrary();

	UFUNCTION(BlueprintPure, Category = "Value")
	static UPositionClaimer* ClaimSelfPosition() { return SelfPosition; };
	UFUNCTION(BlueprintPure, Category = "Value")
	static UPositionClaimer* ClaimSelfForwardPosition() { return SelfForwardPosition; };
	UFUNCTION(BlueprintPure, Category = "Value")
	static UPositionClaimer* ClaimSelfBackwardPosition() { return SelfBackwardPosition; };
	UFUNCTION(BlueprintPure, Category = "Value")
	static UPositionClaimer* ClaimSelfRightPosition() { return SelfRightPosition; };
	UFUNCTION(BlueprintPure, Category = "Value")
	static UPositionClaimer* ClaimSelfLeftPosition() { return SelfLeftPosition; };
	UFUNCTION(BlueprintPure, Category = "Value")
	static UPositionClaimer* ClaimSelfUpPosition() { return SelfUpPosition; };
	UFUNCTION(BlueprintPure, Category = "Value")
	static UPositionClaimer* ClaimSelfDownPosition() { return SelfDownPosition; };

	UFUNCTION(BlueprintPure, Category = "Value", Meta = (DefaultToSelf = "Owner"))
	static UPositionClaimer* MakePositionClaimer(UObject* Owner, FName WantPositionTag, EPositionGetterType WantPositionType, EPositionSpaceType WantAdditiveSpace, UVectorGetter* WantAdditivePosition)
	{
		UPositionClaimer* Result = NewObject<UPositionClaimer>(Owner);
		if (Result) Result->Set(WantPositionTag,WantPositionType,WantAdditiveSpace,WantAdditivePosition);
		return Result;
	}

	UFUNCTION(BlueprintPure, Category = "Value", Meta = (DefaultToSelf = "Owner"))
	static UDirectionClaimer_ToPosition* MakeDirectionClaimer_ToPosition(UObject* Owner, UPositionClaimer* WantFrom, UPositionClaimer* WantTo, UFloatGetter* WantAngleShift)
	{
		UDirectionClaimer_ToPosition* Result = NewObject<UDirectionClaimer_ToPosition>(Owner);
		if (Result) Result->Set(WantFrom, WantTo, WantAngleShift);
		return Result;
	}

	UFUNCTION(BlueprintPure, Category = "Value", Meta = (DefaultToSelf = "Owner"))
	static UDirectionClaimer_SavedDirection* MakeDirectionClaimer_SavedDirection(UObject* Owner, UPositionClaimer* WantFrom, FName WantTag, UFloatGetter* WantAngleShift)
	{
		UDirectionClaimer_SavedDirection* Result = NewObject<UDirectionClaimer_SavedDirection>(Owner);
		if (Result) Result->Set(WantFrom, WantTag, WantAngleShift);
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

	UFUNCTION(BlueprintPure, Category = "Value", Meta = (DefaultToSelf = "Owner"))
	static UActorOrPositionClaimer* MakeActorOrPositionClaimer(UObject* Owner, UPositionClaimer* WantPosition, UActorClaimer* WantActor)
	{
		UActorOrPositionClaimer* Result = NewObject<UActorOrPositionClaimer>(Owner);
		if (Result) Result->Set(WantPosition, WantActor);
		return Result;
	}
};