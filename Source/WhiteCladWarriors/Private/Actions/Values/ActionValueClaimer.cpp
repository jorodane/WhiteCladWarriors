// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/Values/ActionValueClaimer.h"
#include "Actions/Executables/ActionExecutor.h"
#include "Objects/Selectables/Components/UnitActionComponent.h"
#include "Objects/Selectables/Components/UnitMainComponent.h"
#include "Objects/Players/Operator.h"

FVector UPositionClaimer::GetPosition(const FActionCursorFinder& WantCursor, const UUnitActionComponent* Component, const FVector& DefaultValue) const
{
	return GetAdditivePosition(Component);

	//FVector Result = DefaultValue;
	//UActionExecutor* Executor = WantCursor.CurrentExecutor;

	//case EPositionGetterType::CursorPosition:
	//{
	//	if (IsValid(Executor))
	//	{
	//		const FInputPackage& LocalInput = Executor->Operator->GetInputPackage();
	//		Result = LocalInput.MouseTerrainPosition;
	//	}
	//}
}

FVector UPositionClaimer::GetAdditivePosition(const UUnitActionComponent* Component) const
{
	FVector Result;
	if (IsValid(AdditivePosition)) Result = AdditivePosition->GetValue();
	else
	{
		Result = FVector::ZeroVector;
		return Result;
	}

	if (AdditiveSpace == EPositionSpaceType::Self && IsValid(Component))
	{
		UUnitMainComponent* ResultUnit;
		if (Component->TryGetOwnerUnit(ResultUnit))
		{
			return ResultUnit->GetOwner()->GetActorTransform().TransformVector(Result);
		}
	}
	return Result;
}

FVector UPositionClaimer_AveragePosition::GetPosition(const FActionCursorFinder& WantCursor, const UUnitActionComponent* Component, const FVector& DefaultValue) const
{
	FVector Result = DefaultValue;

	FVector SumVector = FVector::ZeroVector;
	int SumCount = 0;
	if (!TargetPositions.IsEmpty())
	{
		for (UPositionClaimer* CurrentPosition : TargetPositions)
		{
			if (!IsValid(CurrentPosition)) continue;
			SumVector += CurrentPosition->GetPosition(WantCursor, Component, DefaultValue);
			SumCount++;
		}
	}
	if (SumCount > 0) Result = SumVector / SumCount;
	if (AdditivePosition) Result += GetAdditivePosition(Component);
	return Result;
}

FVector UPositionClaimer_SelfPosition::GetPosition(const FActionCursorFinder& WantCursor, const UUnitActionComponent* Component, const FVector& DefaultValue) const
{
	FVector Result = DefaultValue;
	UUnitMainComponent* ResultUnit;
	if (IsValid(Component) && Component->TryGetOwnerUnit(ResultUnit)) Result = ResultUnit->GetOwner()->GetActorLocation();
	if (AdditivePosition) Result += GetAdditivePosition(Component);
	return Result;
}

FVector UPositionClaimer_SavedPosition::GetPosition(const FActionCursorFinder& WantCursor, const UUnitActionComponent* Component, const FVector& DefaultValue) const
{
	FVector Result = DefaultValue;
	UActionExecutor* Executor = WantCursor.CurrentExecutor;
	if (IsValid(Executor)) Result = Executor->GetSavedPosition(WantCursor, PositionTag);
	if (AdditivePosition) Result += GetAdditivePosition(Component);
	return Result;
}

FVector UPositionClaimer_ActorPosition::GetPosition(const FActionCursorFinder& WantCursor, const UUnitActionComponent* Component, const FVector& DefaultValue) const
{
	FVector Result = DefaultValue;
	UActionExecutor* Executor = WantCursor.CurrentExecutor;
	if (IsValid(Executor))
	{
		AActor* ResultActor = Executor->GetSavedActor(WantCursor, PositionTag);
		if (IsValid(ResultActor)) Result = ResultActor->GetActorLocation();
	}
	if (AdditivePosition) Result += GetAdditivePosition(Component);
	return Result;
}

FVector UPositionClaimer_SocketPosition::GetPosition(const FActionCursorFinder& WantCursor, const UUnitActionComponent* Component, const FVector& DefaultValue) const
{
	FVector Result = DefaultValue;
	if (IsValid(Component))
	{
		UUnitMainComponent* OwnerUnit;
		if(Component->TryGetOwnerUnit(OwnerUnit) && IsValid(OwnerUnit)) Result = OwnerUnit->GetMesh()->GetSocketLocation(PositionTag);
	}
	if (AdditivePosition) Result += GetAdditivePosition(Component);
	return Result;
}



FVector UDirectionClaimer::GetPosition(const UPositionClaimer* Claimer, const FActionCursorFinder& WantCursor, const UUnitActionComponent* Component, const FVector& DefaultValue) const
{
	if (!IsValid(Claimer))
	{
		if (IsValid(Component))
		{
			if (UUnitMainComponent* Unit = Component->GetOwnerUnit()) return Unit->GetOwner()->GetActorLocation();
		}
		return DefaultValue;
	}

	return Claimer->GetPosition(WantCursor, Component, DefaultValue);
}

FVector UDirectionClaimer::GetDirection(const FActionCursorFinder& WantCursor, const UUnitActionComponent* Component, const FVector& DefaultPosition, const FVector& DefaultDirection) const
{
	FVector Result = GetOriginDirection(WantCursor, Component, DefaultPosition, DefaultDirection);
	if (IsValid(AngleShift))
	{
		FRotator Rotator(0.0, AngleShift->GetValue(), 0.0);
		Result = Rotator.RotateVector(Result);
	}
	return Result;
}

FVector UDirectionClaimer_ToPosition::GetOriginDirection(const FActionCursorFinder& WantCursor, const UUnitActionComponent* Component, const FVector& DefaultPosition, const FVector& DefaultDirection) const
{
	return (GetEndPosition(WantCursor, Component, DefaultPosition, DefaultDirection) - GetStartPosition(WantCursor, Component, DefaultPosition)).GetSafeNormal();
}

FVector UDirectionClaimer_SavedDirection::GetOriginDirection(const FActionCursorFinder& WantCursor, const UUnitActionComponent* Component, const FVector& DefaultPosition, const FVector& DefaultDirection) const
{
	UActionExecutor* Executor = WantCursor.CurrentExecutor;
	if (IsValid(Executor) && !DirectionTag.IsNone())return Executor->GetSavedDirection(WantCursor, DirectionTag);
	else return DefaultDirection;
}

AActor* UActorClaimer::GetActor(const FActionCursorFinder& WantCursor, const UUnitActionComponent* Component) const
{
	UActionExecutor* Executor = WantCursor.CurrentExecutor;
	if (!IsValid(Executor)) return nullptr;
	return Executor->GetSavedActor(WantCursor, ActorTag);
}

TArray<AActor*> UActorArrayClaimer::GetActorArray(const FActionCursorFinder& WantCursor, const UUnitActionComponent* Component) const
{
	UActionExecutor* Executor = WantCursor.CurrentExecutor;
	if (!IsValid(Executor)) return TArray<AActor*>();
	return Executor->GetSavedActorArray(WantCursor, ActorArrayTag);
}

void UValueGetterLibrary::InitSample()
{
	if (IsValid(ForwardVector)) return;

	ZeroVector = NewObject<UVectorGetter_Simple>(this, TEXT("ZeroVector"));
	ZeroVector->SetValue(FVector::ZeroVector);

	ForwardVector = NewObject<UVectorGetter_Simple>(this, TEXT("ForwardVector"));
	ForwardVector->SetValue(FVector::ForwardVector);

	BackwardVector = NewObject<UVectorGetter_Simple>(this, TEXT("BackwardVector"));
	BackwardVector->SetValue(FVector::BackwardVector);

	RightVector = NewObject<UVectorGetter_Simple>(this, TEXT("RightVector"));
	RightVector->SetValue(FVector::RightVector);

	LeftVector = NewObject<UVectorGetter_Simple>(this, TEXT("LeftVector"));
	LeftVector->SetValue(FVector::LeftVector);

	UpVector = NewObject<UVectorGetter_Simple>(this, TEXT("UpVector"));
	UpVector->SetValue(FVector::UpVector);

	DownVector = NewObject<UVectorGetter_Simple>(this, TEXT("DownVector"));
	DownVector->SetValue(FVector::DownVector);
}

void UValueClaimerLibrary::InitSample()
{
	if (IsValid(SelfPosition)) return;

	SelfPosition = NewObject<UPositionClaimer>(this, TEXT("SelfPosition"));
	SelfPosition->Set(EPositionSpaceType::Self, UValueGetterLibrary::GetSimpleZeroVector());

	SelfForwardPosition = NewObject<UPositionClaimer>(this, TEXT("SelfForwardPosition"));
	SelfForwardPosition->Set(EPositionSpaceType::Self, UValueGetterLibrary::GetSimpleForwardVector());

	SelfBackwardPosition = NewObject<UPositionClaimer>(this, TEXT("SelfBackwardPosition"));
	SelfBackwardPosition->Set(EPositionSpaceType::Self, UValueGetterLibrary::GetSimpleBackwardVector());

	SelfRightPosition = NewObject<UPositionClaimer>(this, TEXT("SelfRightPosition"));
	SelfRightPosition->Set(EPositionSpaceType::Self, UValueGetterLibrary::GetSimpleRightVector());

	SelfLeftPosition = NewObject<UPositionClaimer>(this, TEXT("SelfLeftPosition"));
	SelfLeftPosition->Set(EPositionSpaceType::Self, UValueGetterLibrary::GetSimpleLeftVector());

	SelfUpPosition = NewObject<UPositionClaimer>(this, TEXT("SelfUpPosition"));
	SelfUpPosition->Set(EPositionSpaceType::Self, UValueGetterLibrary::GetSimpleUpVector());

	SelfDownPosition = NewObject<UPositionClaimer>(this, TEXT("SelfDownPosition"));
	SelfDownPosition->Set(EPositionSpaceType::Self, UValueGetterLibrary::GetSimpleDownVector());
}