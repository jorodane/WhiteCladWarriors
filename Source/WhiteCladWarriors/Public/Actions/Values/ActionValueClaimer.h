// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Kismet/KismetMathLibrary.h"
#include "ActionValueClaimer.generated.h"

class AActionBase;
class UActorClaimer;
class UActionExecutor;
class UUnitActionComponent;

/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class UFloatGetter : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value", Meta = (ExposeOnSpawn = "true"))
	float Value;

	UFUNCTION(BlueprintPure, Category = "Value")
	virtual float GetValue() const { return Value; }
};

UCLASS(Blueprintable, BlueprintType)
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

UCLASS(Blueprintable, BlueprintType)
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

UCLASS(Abstract, Blueprintable, BlueprintType)
class UVectorGetter : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value", Meta = (ExposeOnSpawn = "true"))
	FVector Value;

	UFUNCTION(BlueprintPure, Category = "Value")
	virtual FVector GetValue() const { return Value; }
};

UCLASS(Blueprintable, BlueprintType)
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

UCLASS(Blueprintable, BlueprintType)
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

UCLASS(Blueprintable, BlueprintType)
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
		//const float r = FMath::Pow(FMath::FRand(), 1.f / 3.f);
		FVector Result = FMath::VRand();
		Result *= Bound;
		Result += Value;
		return Result;
	}
};

UCLASS()
class UValueGetterLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	UPROPERTY()	TObjectPtr<UVectorGetter_Simple> ZeroVector;
	UPROPERTY()	TObjectPtr<UVectorGetter_Simple> ForwardVector;
	UPROPERTY()	TObjectPtr<UVectorGetter_Simple> BackwardVector;
	UPROPERTY()	TObjectPtr<UVectorGetter_Simple> RightVector;
	UPROPERTY()	TObjectPtr<UVectorGetter_Simple> LeftVector;
	UPROPERTY()	TObjectPtr<UVectorGetter_Simple> UpVector;
	UPROPERTY()	TObjectPtr<UVectorGetter_Simple> DownVector;


public:
	void InitSample();

	static inline UValueGetterLibrary* Get()
	{
		UValueGetterLibrary* Result = GetMutableDefault<UValueGetterLibrary>();
		Result->InitSample();
		return Result;
	}

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
		UVectorGetter_RandomSphere* Result = NewObject<UVectorGetter_RandomSphere>(Owner);
		if (Result) Result->SetValue(Center, Bound);
		return Result;
	}

	UFUNCTION(BlueprintPure, Category = "Value")
	static UVectorGetter* GetSimpleZeroVector() { return Get()->ZeroVector; }
	UFUNCTION(BlueprintPure, Category = "Value")
	static UVectorGetter* GetSimpleForwardVector() { return Get()->ForwardVector; }
	UFUNCTION(BlueprintPure, Category = "Value")
	static UVectorGetter* GetSimpleBackwardVector() { return Get()->BackwardVector; }
	UFUNCTION(BlueprintPure, Category = "Value")
	static UVectorGetter* GetSimpleRightVector() { return Get()->RightVector; }
	UFUNCTION(BlueprintPure, Category = "Value")
	static UVectorGetter* GetSimpleLeftVector() { return Get()->LeftVector; }
	UFUNCTION(BlueprintPure, Category = "Value")
	static UVectorGetter* GetSimpleUpVector() { return Get()->UpVector; }
	UFUNCTION(BlueprintPure, Category = "Value")
	static UVectorGetter* GetSimpleDownVector() { return Get()->DownVector;}
};





















UENUM(Blueprintable, BlueprintType)
enum class EPositionSpaceType : uint8 { Self, World };

UCLASS(Abstract, Blueprintable, BlueprintType)
class UValueClaimer : public UObject
{
	GENERATED_BODY()

public:
	static const FHitResult* GetHitResult(const FActionCursorFinder& WantCursor);
};


UCLASS(Abstract, Blueprintable, BlueprintType)
class UFloatClaimer : public UValueClaimer
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Value")
	virtual float GetValue(const FActionCursorFinder& WantCursor, const float DefaultValue) const;
};


UCLASS(Blueprintable, BlueprintType)
class UFloatClaimer_SimpleFloat : public UFloatClaimer
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, Category = "Value")
	TObjectPtr<UFloatGetter> Value;

public:
	UFUNCTION(BlueprintCallable, Category = "Value")
	void Set(UFloatGetter* WantValue)
	{
		Value = WantValue;
	}

	virtual float GetValue(const FActionCursorFinder& WantCursor, const float DefaultValue) const override;
};

UCLASS(Blueprintable, BlueprintType)
class UPositionClaimer : public UValueClaimer
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value")
	EPositionSpaceType AdditiveSpace;

	UPROPERTY(BlueprintReadWrite, Category = "Value")
	TObjectPtr<UVectorGetter> AdditivePosition;

public:
	UFUNCTION(BlueprintCallable, Category = "Value")
	void Set(EPositionSpaceType WantAdditiveSpace, UVectorGetter* WantAdditivePosition)
	{
		AdditiveSpace = WantAdditiveSpace;
		AdditivePosition = WantAdditivePosition;
	}

	UFUNCTION(BlueprintPure, Category = "Value")
	virtual FVector GetPosition(const FActionCursorFinder& WantCursor, const FVector& DefaultValue) const;

	UFUNCTION(BlueprintPure, Category = "Value")
	FVector GetAdditivePosition(const UUnitActionComponent* Component) const;
};

UCLASS(Blueprintable, BlueprintType)
class UPositionClaimer_SimplePosition : public UPositionClaimer
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Value")
	UVectorGetter* TargetPosition;

public:
	void Set(UVectorGetter* WantPosition, EPositionSpaceType WantAdditiveSpace, UVectorGetter* WantAdditivePosition)
	{
		TargetPosition = WantPosition;
		AdditiveSpace = WantAdditiveSpace;
		AdditivePosition = WantAdditivePosition;
	}

	virtual FVector GetPosition(const FActionCursorFinder& WantCursor, const FVector& DefaultValue) const override;
};

UCLASS(Blueprintable, BlueprintType)
class UPositionClaimer_AveragePosition : public UPositionClaimer
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Value")
	UPositionClaimer* PositionLeft;
	UPROPERTY(BlueprintReadWrite, Category = "Value")
	UPositionClaimer* PositionRight;

public:
	void Set(UPositionClaimer* WantLeftPosition, UPositionClaimer* WantRightPosition, EPositionSpaceType WantAdditiveSpace, UVectorGetter* WantAdditivePosition)
	{
		PositionLeft = WantLeftPosition;
		PositionRight = WantRightPosition;
		AdditiveSpace = WantAdditiveSpace;
		AdditivePosition = WantAdditivePosition;
	}

	virtual FVector GetPosition(const FActionCursorFinder& WantCursor, const FVector& DefaultValue) const override;
};

UCLASS(Blueprintable, BlueprintType)
class UPositionClaimer_CombinePosition : public UPositionClaimer
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Value")
	UPositionClaimer* PositionX;
	UPROPERTY(BlueprintReadWrite, Category = "Value")
	UPositionClaimer* PositionY;
	UPROPERTY(BlueprintReadWrite, Category = "Value")
	UPositionClaimer* PositionZ;

public:
	void Set(UPositionClaimer* WantX, UPositionClaimer* WantY, UPositionClaimer* WantZ, EPositionSpaceType WantAdditiveSpace, UVectorGetter* WantAdditivePosition)
	{
		PositionX = WantX;
		PositionY = WantY;
		PositionZ = WantZ;
		AdditiveSpace = WantAdditiveSpace;
		AdditivePosition = WantAdditivePosition;
	}

	virtual FVector GetPosition(const FActionCursorFinder& WantCursor, const FVector& DefaultValue) const override;
};

UCLASS(Blueprintable, BlueprintType)
class UPositionClaimer_SelfPosition : public UPositionClaimer
{
	GENERATED_BODY()

public:
	virtual FVector GetPosition(const FActionCursorFinder& WantCursor, const FVector& DefaultValue) const override;
};

UCLASS(Blueprintable, BlueprintType)
class UPositionClaimer_HitPosition : public UPositionClaimer
{
	GENERATED_BODY()

public:
	void Set(EPositionSpaceType WantAdditiveSpace, UVectorGetter* WantAdditivePosition)
	{
		AdditiveSpace = WantAdditiveSpace;
		AdditivePosition = WantAdditivePosition;
	}

	virtual FVector GetPosition(const FActionCursorFinder& WantCursor, const FVector& DefaultValue) const override;
};

UCLASS(Blueprintable, BlueprintType)
class UPositionClaimer_SavedPosition : public UPositionClaimer
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value")
	FName PositionTag;

public:
	void Set(FName WantPositionTag, EPositionSpaceType WantAdditiveSpace, UVectorGetter* WantAdditivePosition)
	{
		PositionTag = WantPositionTag;
		AdditiveSpace = WantAdditiveSpace;
		AdditivePosition = WantAdditivePosition;
	}

	virtual FVector GetPosition(const FActionCursorFinder& WantCursor, const FVector& DefaultValue) const override;
};

UCLASS(Blueprintable, BlueprintType)
class UPositionClaimer_ActorPosition : public UPositionClaimer
{
	GENERATED_BODY()

public:
	UActorClaimer* TargetActor;

public:

	void Set(UActorClaimer* WantClaimer, EPositionSpaceType WantAdditiveSpace, UVectorGetter* WantAdditivePosition)
	{
		TargetActor = WantClaimer;
		AdditiveSpace = WantAdditiveSpace;
		AdditivePosition = WantAdditivePosition;
	}

	virtual FVector GetPosition(const FActionCursorFinder& WantCursor, const FVector& DefaultValue) const override;
};

UCLASS(Blueprintable, BlueprintType)
class UPositionClaimer_SocketPosition : public UPositionClaimer_SavedPosition
{
	GENERATED_BODY()

public:
	virtual FVector GetPosition(const FActionCursorFinder& WantCursor, const FVector& DefaultValue) const override;
};

UCLASS(Abstract, Blueprintable, BlueprintType)
class UDirectionClaimer : public UValueClaimer
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value")
	TObjectPtr<UFloatGetter> AngleShift;

	UFUNCTION(BlueprintPure, Category = "Value")
	virtual FVector GetPosition(const UPositionClaimer* Claimer, const FActionCursorFinder& WantCursor, const FVector& DefaultValue) const;
	UFUNCTION(BlueprintPure, Category = "Value")
	virtual FVector GetOriginDirection(const FActionCursorFinder& WantCursor, const FVector& DefaultPosition, const FVector& DefaultDirection) const { return DefaultDirection; }

	UFUNCTION(BlueprintPure, Category = "Value")
	virtual FVector GetShiftedDirection(const FVector& OriginDirection) const;

	UFUNCTION(BlueprintPure, Category = "Value")
	virtual FVector GetStartPosition(const FActionCursorFinder& WantCursor, const FVector& DefaultValue) const { return DefaultValue; }
	UFUNCTION(BlueprintPure, Category = "Value")
	virtual FVector GetEndPosition(const FActionCursorFinder& WantCursor, const FVector& DefaultPosition, const FVector& DefaultDirection, float Radius = 1.0f) const { return DefaultPosition + (DefaultDirection * Radius); }
	UFUNCTION(BlueprintPure, Category = "Value")
	virtual FVector GetDirection(const FActionCursorFinder& WantCursor, const FVector& DefaultPosition, const FVector& DefaultDirection) const;
};

UCLASS(Blueprintable, BlueprintType)
class UDirectionClaimer_HitNormal : public UDirectionClaimer
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Value")
	void Set(UFloatGetter* WantAngleShift)
	{
		AngleShift = WantAngleShift;
	}

public:
	virtual FVector GetPosition(const UPositionClaimer* Claimer, const FActionCursorFinder& WantCursor, const FVector& DefaultValue) const override;
	virtual FVector GetOriginDirection(const FActionCursorFinder& WantCursor, const FVector& DefaultPosition, const FVector& DefaultDirection) const override;
	virtual FVector GetStartPosition(const FActionCursorFinder& WantCursor, const FVector& DefaultValue) const override;
	virtual FVector GetEndPosition(const FActionCursorFinder& WantCursor, const FVector& DefaultPosition, const FVector& DefaultDirection, float Radius = 1.0f) const override;
};

UCLASS(Blueprintable, BlueprintType)
class UDirectionClaimer_FromPosition : public UDirectionClaimer
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value")
	TObjectPtr<UPositionClaimer> From;

	virtual FVector GetStartPosition(const FActionCursorFinder& WantCursor, const FVector& DefaultValue) const override;
	virtual FVector GetEndPosition(const FActionCursorFinder& WantCursor, const FVector& DefaultPosition, const FVector& DefaultDirection, float Radius = 1.0f) const override;
};

UCLASS(Blueprintable, BlueprintType)
class UDirectionClaimer_ToPosition : public UDirectionClaimer_FromPosition
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
	virtual FVector GetOriginDirection(const FActionCursorFinder& WantCursor, const FVector& DefaultPosition, const FVector& DefaultDirection) const override;
	virtual FVector GetEndPosition(const FActionCursorFinder& WantCursor, const FVector& DefaultPosition, const FVector& DefaultDirection, float Radius = 1.0f) const override { return GetPosition(To, WantCursor, DefaultPosition); }
};

UCLASS(Blueprintable, BlueprintType)
class UDirectionClaimer_SimpleDirection : public UDirectionClaimer_FromPosition
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value")
	EPositionSpaceType DirectionSpace;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value")
	TObjectPtr<UVectorGetter> DirectionTo;

	UFUNCTION(BlueprintCallable, Category = "Value")
	void Set(UPositionClaimer* WantFrom, EPositionSpaceType WantSpace, UVectorGetter* WantDirection, UFloatGetter* WantAngleShift = nullptr)
	{
		From = WantFrom;
		DirectionSpace = WantSpace;
		DirectionTo = WantDirection;
		AngleShift = WantAngleShift;
	}
	virtual FVector GetOriginDirection(const FActionCursorFinder& WantCursor, const FVector& DefaultPosition, const FVector& DefaultDirection) const override
	{
		if (IsValid(DirectionTo)) return DirectionTo->GetValue();
		else return DefaultDirection;
	}
};

UCLASS(Blueprintable, BlueprintType)
class UDirectionClaimer_SavedDirection : public UDirectionClaimer_FromPosition
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
	virtual FVector GetOriginDirection(const FActionCursorFinder& WantCursor, const FVector& DefaultPosition, const FVector& DefaultDirection) const override;
};

UCLASS(Blueprintable, BlueprintType)
class UActionClaimer : public UValueClaimer
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value")
	FName ActionTag;

	UFUNCTION(BlueprintCallable, Category = "Value")
	void Set(FName WantTag)
	{
		ActionTag = WantTag;
	}

	UFUNCTION(BlueprintPure, Category = "Value")
	virtual AActionBase* GetAction(const FActionCursorFinder& WantCursor) const;
};

UCLASS(Blueprintable, BlueprintType)
class UActionClaimer_UnitTagged : public UActionClaimer
{
	GENERATED_BODY()

public:
	virtual AActionBase* GetAction(const FActionCursorFinder& WantCursor) const override;
};

UCLASS(Blueprintable, BlueprintType)
class UActorClaimer : public UValueClaimer
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Value")
	virtual AActor* GetActor(const FActionCursorFinder& WantCursor) const { return nullptr; }
};

UCLASS(Blueprintable, BlueprintType)
class UActorClaimer_SelfActor : public UActorClaimer
{
	GENERATED_BODY()

public:
	virtual AActor* GetActor(const FActionCursorFinder& WantCursor) const override;
};


UCLASS(Blueprintable, BlueprintType)
class UActorClaimer_SavedActor : public UActorClaimer
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

	virtual AActor* GetActor(const FActionCursorFinder& WantCursor) const override;
};

UCLASS(Blueprintable, BlueprintType)
class UActorClaimer_HitActor : public UActorClaimer
{
	GENERATED_BODY()

public:
	virtual AActor* GetActor(const FActionCursorFinder& WantCursor) const override;
};

UCLASS(Blueprintable, BlueprintType)
class UActorClaimer_TriggerActor : public UActorClaimer
{
	GENERATED_BODY()

public:
	virtual AActor* GetActor(const FActionCursorFinder& WantCursor) const override;
};



UCLASS(Blueprintable, BlueprintType)
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

	UFUNCTION(BlueprintPure, Category = "Value")
	TArray<AActor*> GetActorArray(const FActionCursorFinder& WantCursor) const;
};

UENUM(Blueprintable, BlueprintType)
enum class EActorPositionResult : uint8
{
	Position, Actor
};

UCLASS(Blueprintable, BlueprintType)
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
	inline FVector GetPosition(const FActionCursorFinder& WantCursor, const FVector& DefaultValue) const { if (IsValid(PositionClaimer)) return PositionClaimer->GetPosition(WantCursor, DefaultValue); return DefaultValue; };

	UFUNCTION(BlueprintCallable, Category = "Value")
	inline AActor* GetActor(const FActionCursorFinder& WantCursor) const { if (IsValid(ActorClaimer)) return ActorClaimer->GetActor(WantCursor); return nullptr; };

	UFUNCTION(BlueprintCallable, Category = "Value", meta = (ExpandEnumAsExecs = "ReturnValue"))
	EActorPositionResult TryGetActorOrPosition(const FActionCursorFinder& WantCursor, FVector& ResultPosition, AActor*& ResultAsActor, const FVector& DefaultValue) const
	{
		ResultAsActor = GetActor(WantCursor);
		if (ResultAsActor)
		{
			ResultPosition = ResultAsActor->GetActorLocation();
			return EActorPositionResult::Actor;
		}
		else
		{
			ResultPosition = GetPosition(WantCursor, DefaultValue);
			return EActorPositionResult::Position;
		}
	}
};

UCLASS()
class UValueClaimerLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	UPROPERTY() TObjectPtr<UPositionClaimer> SelfPosition;

	UPROPERTY() TObjectPtr<UPositionClaimer> SelfForwardPosition;
	UPROPERTY() TObjectPtr<UPositionClaimer> SelfBackwardPosition;
	UPROPERTY() TObjectPtr<UPositionClaimer> SelfRightPosition;
	UPROPERTY() TObjectPtr<UPositionClaimer> SelfLeftPosition;
	UPROPERTY() TObjectPtr<UPositionClaimer> SelfUpPosition;
	UPROPERTY() TObjectPtr<UPositionClaimer> SelfDownPosition;
	UPROPERTY() TObjectPtr<UPositionClaimer_HitPosition> HitPosition;

	UPROPERTY() TObjectPtr<UDirectionClaimer_SimpleDirection> SelfForwardDirection;
	UPROPERTY() TObjectPtr<UDirectionClaimer_SimpleDirection> SelfBackwardDirection;
	UPROPERTY() TObjectPtr<UDirectionClaimer_SimpleDirection> SelfRightDirection;
	UPROPERTY() TObjectPtr<UDirectionClaimer_SimpleDirection> SelfLeftDirection;
	UPROPERTY() TObjectPtr<UDirectionClaimer_SimpleDirection> SelfUpDirection;
	UPROPERTY() TObjectPtr<UDirectionClaimer_SimpleDirection> SelfDownDirection;
	UPROPERTY() TObjectPtr<UDirectionClaimer_HitNormal> HitNormal;
	UPROPERTY() TObjectPtr<UDirectionClaimer_HitNormal> HitDirection;

	UPROPERTY() TObjectPtr<UActorClaimer_SelfActor>	 SelfActor;

	UPROPERTY() TObjectPtr<UActorClaimer_HitActor>	 HitActor;

	UPROPERTY() TObjectPtr<UActorClaimer_TriggerActor>	 TriggerActor;

public:
	void InitSample();
	static inline UValueClaimerLibrary* Get() 
	{ 
		UValueClaimerLibrary* Result = GetMutableDefault<UValueClaimerLibrary>();
		Result->InitSample();
		return Result;
	}

	UFUNCTION(BlueprintPure, Category = "Value")
	static UPositionClaimer* ClaimSelfPosition() { return Get()->SelfPosition; };
	UFUNCTION(BlueprintPure, Category = "Value")
	static UPositionClaimer* ClaimSelfForwardPosition() { return Get()->SelfForwardPosition; };
	UFUNCTION(BlueprintPure, Category = "Value")
	static UPositionClaimer* ClaimSelfBackwardPosition() { return Get()->SelfBackwardPosition; };
	UFUNCTION(BlueprintPure, Category = "Value")
	static UPositionClaimer* ClaimSelfRightPosition() { return Get()->SelfRightPosition; };
	UFUNCTION(BlueprintPure, Category = "Value")
	static UPositionClaimer* ClaimSelfLeftPosition() { return Get()->SelfLeftPosition; };
	UFUNCTION(BlueprintPure, Category = "Value")
	static UPositionClaimer* ClaimSelfUpPosition() { return Get()->SelfUpPosition; };
	UFUNCTION(BlueprintPure, Category = "Value")
	static UPositionClaimer* ClaimSelfDownPosition() { return Get()->SelfDownPosition; };
	UFUNCTION(BlueprintPure, Category = "Value")
	static UPositionClaimer_HitPosition* ClaimHitPosition() { return Get()->HitPosition; };


	UFUNCTION(BlueprintPure, Category = "Value")
	static UDirectionClaimer_HitNormal* ClaimHitNormal() { return Get()->HitNormal; };

	UFUNCTION(BlueprintPure, Category = "Value")
	static UDirectionClaimer_HitNormal* ClaimHitDirection() { return Get()->HitDirection; };


	UFUNCTION(BlueprintPure, Category = "Value")
	static UActorClaimer_SelfActor*	 ClaimSelfActor() { return Get()->SelfActor; };
	UFUNCTION(BlueprintPure, Category = "Value")
	static UActorClaimer_HitActor*	 ClaimHitActor() { return Get()->HitActor; };
	UFUNCTION(BlueprintPure, Category = "Value")
	static UActorClaimer_TriggerActor*	 ClaimTriggerActor() { return Get()->TriggerActor; };



	UFUNCTION(BlueprintPure, Category = "Value", Meta = (DefaultToSelf = "Owner"))
	static UFloatClaimer_SimpleFloat* MakeFloatClaimer_SimpleFloat(UObject* Owner, UFloatGetter* Value)
	{
		UFloatClaimer_SimpleFloat* Result = NewObject<UFloatClaimer_SimpleFloat>(Owner);
		if (Result) Result->Set(Value);
		return Result;
	}

	UFUNCTION(BlueprintPure, Category = "Value", Meta = (DefaultToSelf = "Owner"))
	static UPositionClaimer* MakePositionClaimer(UObject* Owner, EPositionSpaceType WantAdditiveSpace, UVectorGetter* WantAdditivePosition)
	{
		UPositionClaimer* Result = NewObject<UPositionClaimer>(Owner);
		if (Result) Result->Set(WantAdditiveSpace, WantAdditivePosition);
		return Result;
	}

	UFUNCTION(BlueprintPure, Category = "Value", Meta = (DefaultToSelf = "Owner"))
	static UPositionClaimer_SimplePosition* MakePositionClaimer_SimplePosition(UObject* Owner, UVectorGetter* TargetPosition, EPositionSpaceType WantAdditiveSpace, UVectorGetter* WantAdditivePosition)
	{
		UPositionClaimer_SimplePosition* Result = NewObject<UPositionClaimer_SimplePosition>(Owner);
		if (Result) Result->Set(TargetPosition, WantAdditiveSpace, WantAdditivePosition);
		return Result;
	}

	UFUNCTION(BlueprintPure, Category = "Value", Meta = (DefaultToSelf = "Owner"))
	static UPositionClaimer_AveragePosition* MakePositionClaimer_AveragePosition(UObject* Owner, UPositionClaimer* WantPositionLeft, UPositionClaimer* WantPositionRight, EPositionSpaceType WantAdditiveSpace, UVectorGetter* WantAdditivePosition)
	{
		UPositionClaimer_AveragePosition* Result = NewObject<UPositionClaimer_AveragePosition>(Owner);
		if (Result) Result->Set(WantPositionLeft, WantPositionRight, WantAdditiveSpace, WantAdditivePosition);
		return Result;
	}

	UFUNCTION(BlueprintPure, Category = "Value", Meta = (DefaultToSelf = "Owner"))
	static UPositionClaimer_CombinePosition* MakePositionClaimer_CombinePosition(UObject* Owner, UPositionClaimer* WantX, UPositionClaimer* WantY, UPositionClaimer* WantZ, EPositionSpaceType WantAdditiveSpace, UVectorGetter* WantAdditivePosition)
	{
		UPositionClaimer_CombinePosition* Result = NewObject<UPositionClaimer_CombinePosition>(Owner);
		if (Result) Result->Set(WantX, WantY, WantZ, WantAdditiveSpace, WantAdditivePosition);
		return Result;
	}

	UFUNCTION(BlueprintPure, Category = "Value", Meta = (DefaultToSelf = "Owner"))
	static UPositionClaimer_SelfPosition* MakePositionClaimer_SelfPosition(UObject* Owner, EPositionSpaceType WantAdditiveSpace, UVectorGetter* WantAdditivePosition)
	{
		UPositionClaimer_SelfPosition* Result = NewObject<UPositionClaimer_SelfPosition>(Owner);
		if (Result) Result->Set(WantAdditiveSpace, WantAdditivePosition);
		return Result;
	}

	UFUNCTION(BlueprintPure, Category = "Value", Meta = (DefaultToSelf = "Owner"))
	static UPositionClaimer_SavedPosition* MakePositionClaimer_SavedPosition(UObject* Owner, FName WantSavedTag, EPositionSpaceType WantAdditiveSpace, UVectorGetter* WantAdditivePosition)
	{
		UPositionClaimer_SavedPosition* Result = NewObject<UPositionClaimer_SavedPosition>(Owner);
		if (Result) Result->Set(WantSavedTag, WantAdditiveSpace, WantAdditivePosition);
		return Result;
	}

	UFUNCTION(BlueprintPure, Category = "Value", Meta = (DefaultToSelf = "Owner"))
	static UPositionClaimer_ActorPosition* MakePositionClaimer_ActorPosition(UObject* Owner, UActorClaimer* WantActor, EPositionSpaceType WantAdditiveSpace, UVectorGetter* WantAdditivePosition)
	{
		UPositionClaimer_ActorPosition* Result = NewObject<UPositionClaimer_ActorPosition>(Owner);
		if (Result) Result->Set(WantActor, WantAdditiveSpace, WantAdditivePosition);
		return Result;
	}

	UFUNCTION(BlueprintPure, Category = "Value", Meta = (DefaultToSelf = "Owner"))
	static UPositionClaimer_SocketPosition* MakePositionClaimer_SocketPosition(UObject* Owner, FName WantSocketName, EPositionSpaceType WantAdditiveSpace, UVectorGetter* WantAdditivePosition)
	{
		UPositionClaimer_SocketPosition* Result = NewObject<UPositionClaimer_SocketPosition>(Owner);
		if (Result) Result->Set(WantSocketName, WantAdditiveSpace, WantAdditivePosition);
		return Result;
	}

	UFUNCTION(BlueprintPure, Category = "Value", Meta = (DefaultToSelf = "Owner"))
	static UPositionClaimer_HitPosition* MakePositionClaimer_HitPosition(UObject* Owner, EPositionSpaceType WantAdditiveSpace, UVectorGetter* WantAdditivePosition)
	{
		UPositionClaimer_HitPosition* Result = NewObject<UPositionClaimer_HitPosition>(Owner);
		if (Result) Result->Set(WantAdditiveSpace, WantAdditivePosition);
		return Result;
	}

	UFUNCTION(BlueprintPure, Category = "Value", Meta = (DefaultToSelf = "Owner"))
	static UDirectionClaimer_SimpleDirection* MakeDirectionClaimer_SimpleDirection(UObject* Owner, UPositionClaimer* WantFrom, UVectorGetter* DefaultDirection, EPositionSpaceType WantAdditiveSpace, UFloatGetter* WantAngleShift)
	{
		UDirectionClaimer_SimpleDirection* Result = NewObject<UDirectionClaimer_SimpleDirection>(Owner);
		if (Result) Result->Set(WantFrom, WantAdditiveSpace, DefaultDirection,WantAngleShift);
		return Result;
	}

	UFUNCTION(BlueprintPure, Category = "Value", Meta = (DefaultToSelf = "Owner"))
	static UDirectionClaimer_HitNormal* MakeDirectionClaimer_HitNormal(UObject* Owner, UFloatGetter* WantAngleShift)
	{
		UDirectionClaimer_HitNormal* Result = NewObject<UDirectionClaimer_HitNormal>(Owner);
		if (Result) Result->Set(WantAngleShift);
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
	static UActionClaimer* MakeActionClaimer(UObject* Owner, FName WantTag)
	{
		UActionClaimer* Result = NewObject<UActionClaimer>(Owner);
		if (Result) Result->Set(WantTag);
		return Result;
	}

	UFUNCTION(BlueprintPure, Category = "Value", Meta = (DefaultToSelf = "Owner"))
	static UActionClaimer_UnitTagged* MakeActionClaimer_UnitTagged(UObject* Owner, FName WantTag)
	{
		UActionClaimer_UnitTagged* Result = NewObject<UActionClaimer_UnitTagged>(Owner);
		if (Result) Result->Set(WantTag);
		return Result;
	}

	UFUNCTION(BlueprintPure, Category = "Value", Meta = (DefaultToSelf = "Owner"))
	static UActorClaimer_SavedActor* MakeActorClaimer_SavedActor(UObject* Owner, FName WantTag)
	{
		UActorClaimer_SavedActor* Result = NewObject<UActorClaimer_SavedActor>(Owner);
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