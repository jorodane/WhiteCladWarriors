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
	FActiveNodeInfo* Info = Executor->GetNodeInfo(WantCursor);
	if (Info == nullptr) return nullptr;
	return nullptr;
}

float UFloatClaimer::GetValue(const FActionCursorFinder& WantCursor, const float DefaultValue) const
{
	return DefaultValue;
}

float UFloatClaimer_SimpleFloat::GetValue(const FActionCursorFinder& WantCursor, const float DefaultValue) const
{
	if (!IsValid(Value)) return DefaultValue;
	return Value->GetValue();
}

float UFloatClaimer_PositionDistance::GetValue(const FActionCursorFinder& WantCursor, const float DefaultValue) const
{
	if (!IsValid(From) || !IsValid(To)) return DefaultValue;
	FVector FromPosition = From->GetPosition(WantCursor, FVector::ZeroVector);
	FVector ToPosition = To->GetPosition(WantCursor, FVector::ZeroVector);
	return FVector::Dist(FromPosition, ToPosition);
}

float UFloatClaimer_DirectionDistance::GetValue(const FActionCursorFinder& WantCursor, const float DefaultValue) const
{
	if (!IsValid(Direction)) return DefaultValue;
	return Direction->GetDistance(WantCursor);
}


FVector UPositionClaimer::GetPosition(const FActionCursorFinder& WantCursor, const FVector& DefaultValue) const
{
	return GetAdditivePosition(WantCursor.CurrentComponent);
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

FVector UPositionClaimer_SimplePosition::GetPosition(const FActionCursorFinder& WantCursor, const FVector& DefaultValue) const
{
	FVector Result = IsValid(TargetPosition)? TargetPosition->GetValue() : DefaultValue;
	if (AdditivePosition) Result += GetAdditivePosition(WantCursor.CurrentComponent);
	return Result;
}

FVector UPositionClaimer_AveragePosition::GetPosition(const FActionCursorFinder& WantCursor, const FVector& DefaultValue) const
{
	FVector Result = DefaultValue;

	FVector SumVector = FVector::ZeroVector;
	int SumCount = 0;
	if (IsValid(PositionLeft))
	{
		SumVector+=PositionLeft->GetPosition(WantCursor, DefaultValue);
		++SumCount;
	}
	if (IsValid(PositionRight))
	{
		SumVector += PositionRight->GetPosition(WantCursor, DefaultValue);
		++SumCount;
	}
	if (SumCount > 0) Result = SumVector / SumCount;
	if (AdditivePosition) Result += GetAdditivePosition(WantCursor.CurrentComponent);
	return Result;
}

FVector UPositionClaimer_CombinePosition::GetPosition(const FActionCursorFinder& WantCursor, const FVector& DefaultValue) const
{
	FVector Result = DefaultValue;

	bool SameXY = PositionX == PositionY;
	bool SameXZ = PositionX == PositionZ;
	bool SameYZ = PositionY == PositionZ;
	bool ValidX = IsValid(PositionX);

	if (SameXY && SameXZ && PositionX == nullptr && ValidX) return Result;

	FVector ResultX = DefaultValue;
	if (ValidX) ResultX = PositionX->GetPosition(WantCursor, DefaultValue);
	else		ResultX = DefaultValue;
	FVector ResultY;
	if (SameXY) ResultY = ResultX;
	else if (IsValid(PositionY)) ResultY = PositionY->GetPosition(WantCursor, DefaultValue);
	else		ResultY = DefaultValue;
	FVector ResultZ;
	if (SameXZ) ResultZ = ResultX;
	if (SameYZ) ResultZ = ResultY;
	else if (IsValid(PositionZ)) ResultZ = PositionZ->GetPosition(WantCursor, DefaultValue);
	else		ResultZ = DefaultValue;

	Result.X = ResultX.X;
	Result.Y = ResultY.Y;
	Result.Z = ResultZ.Z;

	if (AdditivePosition) Result += GetAdditivePosition(WantCursor.CurrentComponent);
	return Result;
}

FVector UPositionClaimer_SelfPosition::GetPosition(const FActionCursorFinder& WantCursor, const FVector& DefaultValue) const
{
	FVector Result = DefaultValue;
	UUnitMainComponent* ResultUnit;
	if (IsValid(WantCursor.CurrentComponent) && WantCursor.CurrentComponent->TryGetOwnerUnit(ResultUnit)) Result = ResultUnit->GetOwner()->GetActorLocation();
	if (AdditivePosition) Result += GetAdditivePosition(WantCursor.CurrentComponent);
	return Result;
}

FVector UPositionClaimer_HitPosition::GetPosition(const FActionCursorFinder& WantCursor, const FVector& DefaultValue) const
{
	const FHitResult* HitResult = UValueClaimer::GetHitResult(WantCursor);
	if (HitResult == nullptr) return DefaultValue;
	return HitResult->Location;
}

FVector UPositionClaimer_SavedPosition::GetPosition(const FActionCursorFinder& WantCursor, const FVector& DefaultValue) const
{
	FVector Result = DefaultValue;
	bool bIsValidValueMap = false;
	FExecutorValueMap& ValueMap = UActionExecutor::GetValueMapFromCursor(WantCursor, bIsValidValueMap);
	if(bIsValidValueMap) Result = ValueMap.GetSavedPosition(WantCursor, PositionTag);
	if (AdditivePosition) Result += GetAdditivePosition(WantCursor.CurrentComponent);
	return Result;
}

FVector UPositionClaimer_ActorPosition::GetPosition(const FActionCursorFinder& WantCursor, const FVector& DefaultValue) const
{
	FVector Result;

	if (IsValid(TargetActor))
	{
		if (AActor* ResultActor = TargetActor->GetActor(WantCursor))
		{
			if (IsValid(ResultActor)) Result = ResultActor->GetActorLocation();
		}
	}
	else Result = DefaultValue;

	if (AdditivePosition) Result += GetAdditivePosition(WantCursor.CurrentComponent);
	return Result;
}

FVector UPositionClaimer_SocketPosition::GetPosition(const FActionCursorFinder& WantCursor, const FVector& DefaultValue) const
{
	FVector Result = DefaultValue;
	if (IsValid(WantCursor.CurrentComponent))
	{
		UUnitMainComponent* OwnerUnit;
		if (WantCursor.CurrentComponent->TryGetOwnerUnit(OwnerUnit) && IsValid(OwnerUnit)) Result = OwnerUnit->GetMesh()->GetSocketLocation(PositionTag);
	}
	if (AdditivePosition) Result += GetAdditivePosition(WantCursor.CurrentComponent);
	return Result;
}


FVector UDirectionClaimer::GetPosition(const FActionCursorFinder& WantCursor, const FVector& DefaultValue) const
{
	if (IsValid(WantCursor.CurrentComponent))
	{
		if (UUnitMainComponent* Unit = WantCursor.CurrentComponent->GetOwnerUnit()) return Unit->GetOwner()->GetActorLocation();
	}
	return DefaultValue;
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


FVector UDirectionClaimer::GetDirection(const FActionCursorFinder& WantCursor, const FVector& DefaultPosition, const FVector& DefaultDirection) const
{
	return GetShiftedDirection(GetOriginDirection(WantCursor, DefaultPosition, DefaultDirection));
}

FVector UDirectionClaimer_HitNormal::GetPosition(const FActionCursorFinder& WantCursor, const FVector& DefaultValue) const
{
	const FHitResult* HitResult = UValueClaimer::GetHitResult(WantCursor);
	if (HitResult != nullptr) return HitResult->Location;
	return Super::GetPosition(WantCursor, DefaultValue);
}

FVector UDirectionClaimer_HitNormal::GetOriginDirection(const FActionCursorFinder& WantCursor, const FVector& DefaultPosition, const FVector& DefaultDirection) const
{
	const FHitResult* HitResult = UValueClaimer::GetHitResult(WantCursor);
	if (HitResult != nullptr) return HitResult->ImpactNormal;
	return DefaultDirection;
}

FVector UDirectionClaimer_HitNormal::GetEndPosition(const FActionCursorFinder& WantCursor, const FVector& DefaultPosition, const FVector& DefaultDirection, float Radius) const
{
	const FHitResult* HitResult = UValueClaimer::GetHitResult(WantCursor);
	if (HitResult != nullptr) return HitResult->Location + GetShiftedDirection(HitResult->ImpactNormal) * Radius; 
	return Super::GetEndPosition(WantCursor, DefaultPosition, DefaultDirection, Radius);
}

FVector UDirectionClaimer_FromPosition::GetPosition(const FActionCursorFinder& WantCursor, const FVector& DefaultValue) const
{ 
	if (IsValid(From))
	{
		return From->GetPosition(WantCursor, DefaultValue);
	}

	return Super::GetPosition(WantCursor, DefaultValue); 
}

FVector UDirectionClaimer_FromPosition::GetEndPosition(const FActionCursorFinder& WantCursor, const FVector& DefaultPosition, const FVector& DefaultDirection, float Radius) const 
{ 
	return GetPosition(WantCursor, DefaultPosition) + (GetOriginDirection(WantCursor, DefaultPosition, DefaultDirection) * Radius); 
}

FVector UDirectionClaimer_ToPosition::GetOriginDirection(const FActionCursorFinder& WantCursor, const FVector& DefaultPosition, const FVector& DefaultDirection) const
{
	return (GetEndPosition(WantCursor, DefaultPosition, DefaultDirection) - GetPosition(WantCursor, DefaultPosition)).GetSafeNormal();
}

FVector UDirectionClaimer_SavedDirection::GetOriginDirection(const FActionCursorFinder& WantCursor, const FVector& DefaultPosition, const FVector& DefaultDirection) const
{
	bool bIsValidValueMap = false;
	FExecutorValueMap& ValueMap = UActionExecutor::GetValueMapFromCursor(WantCursor, bIsValidValueMap);
	if (bIsValidValueMap && !DirectionTag.IsNone()) return ValueMap.GetSavedDirection(WantCursor, DirectionTag);
	else return DefaultDirection;
}

AActionBase* UActionClaimer::GetAction(const FActionCursorFinder& WantCursor) const
{
	return UActionSetting::GetAction(ActionTag);
}

AActionBase* UActionClaimer_UnitTagged::GetAction(const FActionCursorFinder& WantCursor) const
{
	FName Finder;

	if (UUnitMainComponent* Unit = WantCursor.CurrentComponent->GetOwnerUnit()) Finder = Unit->GetActionFromTag(ActionTag);
	else Finder = ActionTag;

	return UActionSetting::GetAction(Finder);
}

AActor* UActorClaimer_SelfActor::GetActor(const FActionCursorFinder& WantCursor) const
{
	if (!IsValid(WantCursor.CurrentComponent)) return nullptr;

	return WantCursor.CurrentComponent->GetOwner();
}

AActor* UActorClaimer_SavedActor::GetActor(const FActionCursorFinder& WantCursor) const
{
	bool bIsValidValueMap = false;
	FExecutorValueMap& ValueMap = UActionExecutor::GetValueMapFromCursor(WantCursor, bIsValidValueMap);
	if (!bIsValidValueMap) return nullptr;
	return ValueMap.GetSavedActor(WantCursor, ActorTag);
}

AActor* UActorClaimer_HitActor::GetActor(const FActionCursorFinder& WantCursor) const
{
	const FHitResult* HitResult = UValueClaimer::GetHitResult(WantCursor);
	if (HitResult == nullptr) return nullptr;
	return HitResult->GetActor();
}

AActor* UActorClaimer_TriggerActor::GetActor(const FActionCursorFinder& WantCursor) const
{
	if (IsValid(WantCursor.ClaimActor)) return WantCursor.ClaimActor;
	else if (IsValid(WantCursor.CurrentComponent)) return WantCursor.CurrentComponent->GetOwner();
	return nullptr;
}

TArray<AActor*> UActorArrayClaimer::GetActorArray(const FActionCursorFinder& WantCursor) const
{
	bool bIsValidValueMap = false;
	FExecutorValueMap& ValueMap = UActionExecutor::GetValueMapFromCursor(WantCursor, bIsValidValueMap);
	if (!bIsValidValueMap) return TArray<AActor*>();
	return ValueMap.GetSavedActorArray(WantCursor, ActorArrayTag);
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
	SelfForwardDirection->SetDirection(SelfPosition, EPositionSpaceType::Self, UValueGetterLibrary::GetSimpleForwardVector());

	SelfBackwardDirection = NewObject<UDirectionClaimer_SimpleDirection>(this, TEXT("SelfBackwardDirection"));
	SelfBackwardDirection->SetDirection(SelfPosition, EPositionSpaceType::Self, UValueGetterLibrary::GetSimpleBackwardVector());

	SelfRightDirection = NewObject<UDirectionClaimer_SimpleDirection>(this, TEXT("SelfRightDirection"));
	SelfRightDirection->SetDirection(SelfPosition, EPositionSpaceType::Self, UValueGetterLibrary::GetSimpleRightVector());

	SelfLeftDirection = NewObject<UDirectionClaimer_SimpleDirection>(this, TEXT("SelfLeftDirection"));
	SelfLeftDirection->SetDirection(SelfPosition, EPositionSpaceType::Self, UValueGetterLibrary::GetSimpleLeftVector());

	SelfUpDirection = NewObject<UDirectionClaimer_SimpleDirection>(this, TEXT("SelfUpDirection"));
	SelfUpDirection->SetDirection(SelfPosition, EPositionSpaceType::Self, UValueGetterLibrary::GetSimpleUpVector());

	SelfDownDirection = NewObject<UDirectionClaimer_SimpleDirection>(this, TEXT("SelfDownDirection"));
	SelfDownDirection->SetDirection(SelfPosition, EPositionSpaceType::Self, UValueGetterLibrary::GetSimpleDownVector());

	HitNormal = NewObject<UDirectionClaimer_HitNormal>(this, TEXT("HitNormal"));

	HitDirection = NewObject<UDirectionClaimer_HitNormal>(this, TEXT("HitDirection"));
	HitDirection->SetDirection(UValueGetterLibrary::MakeSimpleFloat(this, 180.0f));

	SelfActor = NewObject<UActorClaimer_SelfActor>(this, TEXT("SelfActor"));
	HitActor = NewObject<UActorClaimer_HitActor>(this, TEXT("HitActor"));
	TriggerActor = NewObject<UActorClaimer_TriggerActor>(this, TEXT("TriggerActor"));
}