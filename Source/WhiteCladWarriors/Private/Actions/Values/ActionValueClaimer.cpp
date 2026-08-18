// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/Values/ActionValueClaimer.h"
#include "Actions/Executables/ActionExecutor.h"
#include "Actions/ActionBase.h"
#include "Objects/Selectables/Components/UnitActionComponent.h"
#include "Objects/Selectables/Components/UnitMainComponent.h"
#include "Objects/Players/Operator.h"
#include "Settings/ActionSetting.h"

const FHitResult* UValueClaimer::GetHitResult(const FActionCursorFinder& WantCursor)
{
	UActionExecutor* Executor = UActionExecutor::GetExecutorFromCursor(WantCursor);
	if (!IsValid(Executor)) return nullptr;
	FActiveNodeInfo_Hit* Info = Executor->GetNodeInfo<FActiveNodeInfo_Hit>(WantCursor);
	if (Info == nullptr) return nullptr;
	return &Info->Hit;
}

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
	if (IsValid(PositionLeft))
	{
		SumVector+=PositionLeft->GetPosition(WantCursor, Component, DefaultValue);
		++SumCount;
	}
	if (IsValid(PositionRight))
	{
		SumVector += PositionRight->GetPosition(WantCursor, Component, DefaultValue);
		++SumCount;
	}
	if (SumCount > 0) Result = SumVector / SumCount;
	if (AdditivePosition) Result += GetAdditivePosition(Component);
	return Result;
}

FVector UPositionClaimer_CombinePosition::GetPosition(const FActionCursorFinder& WantCursor, const UUnitActionComponent* Component, const FVector& DefaultValue) const
{
	FVector Result = DefaultValue;

	bool SameXY = PositionX == PositionY;
	bool SameXZ = PositionX == PositionZ;
	bool SameYZ = PositionY == PositionZ;
	bool ValidX = IsValid(PositionX);

	if (SameXY && SameXZ && PositionX == nullptr && ValidX) return Result;

	FVector ResultX = DefaultValue;
	if (ValidX) ResultX = PositionX->GetPosition(WantCursor, Component, DefaultValue);
	else		ResultX = DefaultValue;
	FVector ResultY;
	if (SameXY) ResultY = ResultX;
	else if (IsValid(PositionY)) ResultY = PositionY->GetPosition(WantCursor, Component, DefaultValue);
	else		ResultY = DefaultValue;
	FVector ResultZ;
	if (SameXZ) ResultZ = ResultX;
	if (SameYZ) ResultZ = ResultY;
	else if (IsValid(PositionZ)) ResultZ = PositionZ->GetPosition(WantCursor, Component, DefaultValue);
	else		ResultZ = DefaultValue;

	Result.X = ResultX.X;
	Result.Y = ResultY.Y;
	Result.Z = ResultZ.Z;

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

FVector UPositionClaimer_HitPosition::GetPosition(const FActionCursorFinder& WantCursor, const UUnitActionComponent* Component, const FVector& DefaultValue) const
{
	const FHitResult* HitResult = UValueClaimer::GetHitResult(WantCursor);
	if (HitResult == nullptr) return DefaultValue;
	return HitResult->Location;
}

FVector UPositionClaimer_SavedPosition::GetPosition(const FActionCursorFinder& WantCursor, const UUnitActionComponent* Component, const FVector& DefaultValue) const
{
	FVector Result = DefaultValue;
	UActionExecutor* Executor = UActionExecutor::GetExecutorFromCursor(WantCursor);
	if (IsValid(Executor)) Result = Executor->GetSavedPosition(WantCursor, PositionTag);
	if (AdditivePosition) Result += GetAdditivePosition(Component);
	return Result;
}

FVector UPositionClaimer_ActorPosition::GetPosition(const FActionCursorFinder& WantCursor, const UUnitActionComponent* Component, const FVector& DefaultValue) const
{
	FVector Result;

	if (IsValid(TargetActor))
	{
		if (AActor* ResultActor = TargetActor->GetActor(WantCursor, Component))
		{
			if (IsValid(ResultActor)) Result = ResultActor->GetActorLocation();
		}
	}
	else Result = DefaultValue;

	if (AdditivePosition) Result += GetAdditivePosition(Component);
	return Result;
}

FVector UPositionClaimer_SocketPosition::GetPosition(const FActionCursorFinder& WantCursor, const UUnitActionComponent* Component, const FVector& DefaultValue) const
{
	FVector Result = DefaultValue;
	if (IsValid(Component))
	{
		UUnitMainComponent* OwnerUnit;
		if (Component->TryGetOwnerUnit(OwnerUnit) && IsValid(OwnerUnit)) Result = OwnerUnit->GetMesh()->GetSocketLocation(PositionTag);
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

FVector UDirectionClaimer::GetShiftedDirection(const FVector& OriginDirection) const
{
	FVector Result = OriginDirection;
	if (IsValid(AngleShift))
	{
		FRotator Rotator(0.0, AngleShift->GetValue(), 0.0);
		Result = Rotator.RotateVector(Result);
	}
	return Result;
}


FVector UDirectionClaimer::GetDirection(const FActionCursorFinder& WantCursor, const UUnitActionComponent* Component, const FVector& DefaultPosition, const FVector& DefaultDirection) const
{
	return GetShiftedDirection(GetOriginDirection(WantCursor, Component, DefaultPosition, DefaultDirection));
}

FVector UDirectionClaimer_HitNormal::GetPosition(const UPositionClaimer* Claimer, const FActionCursorFinder& WantCursor, const UUnitActionComponent* Component, const FVector& DefaultValue) const
{
	const FHitResult* HitResult = UValueClaimer::GetHitResult(WantCursor);
	if (HitResult == nullptr) return DefaultValue;
	return HitResult->Location;
}

FVector UDirectionClaimer_HitNormal::GetOriginDirection(const FActionCursorFinder& WantCursor, const UUnitActionComponent* Component, const FVector& DefaultPosition, const FVector& DefaultDirection) const
{
	const FHitResult* HitResult = UValueClaimer::GetHitResult(WantCursor);
	if (HitResult == nullptr) return DefaultDirection;
	return HitResult->ImpactNormal;
}

FVector UDirectionClaimer_HitNormal::GetStartPosition(const FActionCursorFinder& WantCursor, const UUnitActionComponent* Component, const FVector& DefaultValue) const
{
	const FHitResult* HitResult = UValueClaimer::GetHitResult(WantCursor);
	if (HitResult == nullptr) return DefaultValue;
	return HitResult->Location;
}

FVector UDirectionClaimer_HitNormal::GetEndPosition(const FActionCursorFinder& WantCursor, const UUnitActionComponent* Component, const FVector& DefaultPosition, const FVector& DefaultDirection, float Radius) const
{
	const FHitResult* HitResult = UValueClaimer::GetHitResult(WantCursor);
	if (HitResult == nullptr) return DefaultPosition + (DefaultDirection * Radius);
	return HitResult->Location + GetShiftedDirection(HitResult->ImpactNormal) * Radius;
}

FVector UDirectionClaimer_FromPosition::GetStartPosition(const FActionCursorFinder& WantCursor, const UUnitActionComponent* Component, const FVector& DefaultValue) const
{ 
	return GetPosition(From, WantCursor, Component, DefaultValue); 
}

FVector UDirectionClaimer_FromPosition::GetEndPosition(const FActionCursorFinder& WantCursor, const UUnitActionComponent* Component, const FVector& DefaultPosition, const FVector& DefaultDirection, float Radius) const 
{ 
	return GetStartPosition(WantCursor, Component, DefaultPosition) + (GetOriginDirection(WantCursor, Component, DefaultPosition, DefaultDirection) * Radius); 
}

FVector UDirectionClaimer_ToPosition::GetOriginDirection(const FActionCursorFinder& WantCursor, const UUnitActionComponent* Component, const FVector& DefaultPosition, const FVector& DefaultDirection) const
{
	return (GetEndPosition(WantCursor, Component, DefaultPosition, DefaultDirection) - GetStartPosition(WantCursor, Component, DefaultPosition)).GetSafeNormal();
}

FVector UDirectionClaimer_SavedDirection::GetOriginDirection(const FActionCursorFinder& WantCursor, const UUnitActionComponent* Component, const FVector& DefaultPosition, const FVector& DefaultDirection) const
{
	UActionExecutor* Executor = UActionExecutor::GetExecutorFromCursor(WantCursor);
	if (IsValid(Executor) && !DirectionTag.IsNone())return Executor->GetSavedDirection(WantCursor, DirectionTag);
	else return DefaultDirection;
}

AActionBase* UActionClaimer::GetAction(const FActionCursorFinder& WantCursor, const UUnitActionComponent* Component) const
{
	return UActionSetting::GetAction(ActionTag);
}

AActionBase* UActionClaimer_UnitTagged::GetAction(const FActionCursorFinder& WantCursor, const UUnitActionComponent* Component) const
{
	FName Finder;

	if (UUnitMainComponent* Unit = Component->GetOwnerUnit()) Finder = Unit->GetActionFromTag(ActionTag);
	else Finder = ActionTag;

	return UActionSetting::GetAction(Finder);
}

AActor* UActorClaimer_SelfActor::GetActor(const FActionCursorFinder& WantCursor, const UUnitActionComponent* Component) const
{
	if (!IsValid(Component))
	{
		Component = WantCursor.CurrentComponent;
		if (!IsValid(Component)) return nullptr;
	}

	return Component->GetOwner();
}

AActor* UActorClaimer_SavedActor::GetActor(const FActionCursorFinder& WantCursor, const UUnitActionComponent* Component) const
{
	UActionExecutor* Executor = UActionExecutor::GetExecutorFromCursor(WantCursor);
	if (!IsValid(Executor)) return nullptr;
	return Executor->GetSavedActor(WantCursor, ActorTag);
}

AActor* UActorClaimer_HitActor::GetActor(const FActionCursorFinder& WantCursor, const UUnitActionComponent* Component) const
{
	const FHitResult* HitResult = UValueClaimer::GetHitResult(WantCursor);
	if (HitResult == nullptr) return nullptr;
	return HitResult->GetActor();
}

TArray<AActor*> UActorArrayClaimer::GetActorArray(const FActionCursorFinder& WantCursor, const UUnitActionComponent* Component) const
{
	UActionExecutor* Executor = UActionExecutor::GetExecutorFromCursor(WantCursor);
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

	SelfPosition = NewObject<UPositionClaimer_SelfPosition>(this, TEXT("SelfPosition"));

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

	HitPosition = NewObject<UPositionClaimer_HitPosition>(this, TEXT("HitPosition"));

	SelfForwardDirection = NewObject<UDirectionClaimer_SimpleDirection>(this, TEXT("SelfForwardDirection"));
	SelfForwardDirection->Set(SelfPosition, EPositionSpaceType::Self, UValueGetterLibrary::GetSimpleForwardVector());

	SelfBackwardDirection = NewObject<UDirectionClaimer_SimpleDirection>(this, TEXT("SelfBackwardDirection"));
	SelfBackwardDirection->Set(SelfPosition, EPositionSpaceType::Self, UValueGetterLibrary::GetSimpleBackwardVector());

	SelfRightDirection = NewObject<UDirectionClaimer_SimpleDirection>(this, TEXT("SelfRightDirection"));
	SelfRightDirection->Set(SelfPosition, EPositionSpaceType::Self, UValueGetterLibrary::GetSimpleRightVector());

	SelfLeftDirection = NewObject<UDirectionClaimer_SimpleDirection>(this, TEXT("SelfLeftDirection"));
	SelfLeftDirection->Set(SelfPosition, EPositionSpaceType::Self, UValueGetterLibrary::GetSimpleLeftVector());

	SelfUpDirection = NewObject<UDirectionClaimer_SimpleDirection>(this, TEXT("SelfUpDirection"));
	SelfUpDirection->Set(SelfPosition, EPositionSpaceType::Self, UValueGetterLibrary::GetSimpleUpVector());

	SelfDownDirection = NewObject<UDirectionClaimer_SimpleDirection>(this, TEXT("SelfDownDirection"));
	SelfDownDirection->Set(SelfPosition, EPositionSpaceType::Self, UValueGetterLibrary::GetSimpleDownVector());

	HitNormal = NewObject<UDirectionClaimer_HitNormal>(this, TEXT("HitNormal"));

	HitDirection = NewObject<UDirectionClaimer_HitNormal>(this, TEXT("HitDirection")); 
	HitDirection->Set(UValueGetterLibrary::MakeSimpleFloat(this, 180.0f));

	SelfActor = NewObject<UActorClaimer_SelfActor>(this, TEXT("SelfActor"));
	HitActor = NewObject<UActorClaimer_HitActor>(this, TEXT("HitActor"));
}