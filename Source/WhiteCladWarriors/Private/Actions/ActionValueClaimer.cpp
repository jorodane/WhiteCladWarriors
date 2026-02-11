// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/ActionValueClaimer.h"
#include "Actions/ActionExecutor.h"
#include "Objects/Selectables/Components/UnitActionComponent.h"
#include "Objects/Selectables/Units/UnitBase.h"
#include "Objects/Players/Operator.h"

FVector UPositionClaimer::GetPosition(const FActionCursorFinder& WantCursor) const
{
	FVector Result = FVector::ZeroVector;
	UActionExecutor* Executor = WantCursor.CurrentExecutor;
	UUnitActionComponent* Component = WantCursor.CurrentComponent;

	if (IsValid(Executor) && IsValid(Component)) switch (PositionType)
	{
	case EPositionGetterType::SavedPosition:
		Result = Executor->GetSavedPosition(WantCursor, PositionTag);
		break;
	case EPositionGetterType::SelfPosition:
	{
		AUnitBase* ResultUnit;
		if (Component->TryGetOwnerUnit(ResultUnit)) Result = ResultUnit->GetActorLocation();
	}
	break;
	case EPositionGetterType::ActorPosition:
	{
		AActor* ResultActor = Executor->GetSavedActor(WantCursor, PositionTag);
		if (IsValid(ResultActor)) Result = ResultActor->GetActorLocation();
	}
	break;
	case EPositionGetterType::CursorPosition:
	{
		const FInputPackage& LocalInput = Executor->Operator->GetInputPackage();
		Result = LocalInput.MouseTerrainPosition;
	}
	break;
	case EPositionGetterType::WorldPosition:
	default:
		break;
	}
	if (AdditivePosition) Result += GetAdditivePosition(Component);
	return Result;
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
		AUnitBase* ResultUnit;
		if (Component->TryGetOwnerUnit(ResultUnit))
		{
			return ResultUnit->GetActorTransform().TransformVector(Result);
		}
	}
	return Result;
}
FVector UDirectionClaimer::GetPosition(const UPositionClaimer* Claimer, const FActionCursorFinder& WantCursor) const
{
	UActionExecutor* Executor = WantCursor.CurrentExecutor;
	UUnitActionComponent* Component = WantCursor.CurrentComponent;
	if (!IsValid(Executor) || !IsValid(Component)) return FVector::ZeroVector;
	if (!IsValid(Claimer)) if (AUnitBase* Unit = Component->GetOwnerUnit()) Unit->GetActorLocation(); else return FVector::ZeroVector;
	return Claimer->GetPosition(WantCursor);
}

FVector UDirectionClaimer::GetDirection(const FActionCursorFinder& WantCursor) const
{
	FVector Result = GetOriginDirection(WantCursor);
	if (IsValid(AngleShift))
	{
		FRotator Rotator(0.0, AngleShift->GetValue(), 0.0);
		Result = Rotator.RotateVector(Result);
	}
	return Result;
}

FVector UDirectionClaimer_ToPosition::GetOriginDirection(const FActionCursorFinder& WantCursor) const
{
	UActionExecutor* Executor = WantCursor.CurrentExecutor;
	if (!IsValid(Executor)) return FVector::ZeroVector;
	else return (GetEndPosition(WantCursor) - GetStartPosition(WantCursor)).GetSafeNormal();
}
FVector UDirectionClaimer_SavedDirection::GetOriginDirection(const FActionCursorFinder& WantCursor) const
{
	UActionExecutor* Executor = WantCursor.CurrentExecutor;
	if (IsValid(Executor) && !DirectionTag.IsNone())return Executor->GetSavedDirection(WantCursor, DirectionTag);
	else return FVector::ZeroVector;
}

AActor* UActorClaimer::GetActor(const FActionCursorFinder& WantCursor) const
{
	UActionExecutor* Executor = WantCursor.CurrentExecutor;
	if (!IsValid(Executor)) return nullptr;
	return Executor->GetSavedActor(WantCursor, ActorTag);
}

TArray<AActor*> UActorArrayClaimer::GetActorArray(const FActionCursorFinder& WantCursor) const
{
	UActionExecutor* Executor = WantCursor.CurrentExecutor;
	if (!IsValid(Executor)) return TArray<AActor*>();
	return Executor->GetSavedActorArray(WantCursor, ActorArrayTag);
}

void UValueGetterLibrary::InitSample()
{
	if (IsValid(ForwardVector)) return;

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
	SelfPosition->Set(NAME_None, EPositionGetterType::SelfPosition, EPositionSpaceType::World, nullptr);

	SelfForwardPosition = NewObject<UPositionClaimer>(this, TEXT("SelfForwardPosition"));
	SelfForwardPosition->Set(NAME_None, EPositionGetterType::SelfPosition, EPositionSpaceType::Self, UValueGetterLibrary::GetSimpleForwardVector());

	SelfBackwardPosition = NewObject<UPositionClaimer>(this, TEXT("SelfBackwardPosition"));
	SelfBackwardPosition->Set(NAME_None, EPositionGetterType::SelfPosition, EPositionSpaceType::Self, UValueGetterLibrary::GetSimpleBackwardVector());

	SelfRightPosition = NewObject<UPositionClaimer>(this, TEXT("SelfRightPosition"));
	SelfRightPosition->Set(NAME_None, EPositionGetterType::SelfPosition, EPositionSpaceType::Self, UValueGetterLibrary::GetSimpleRightVector());

	SelfLeftPosition = NewObject<UPositionClaimer>(this, TEXT("SelfLeftPosition"));
	SelfLeftPosition->Set(NAME_None, EPositionGetterType::SelfPosition, EPositionSpaceType::Self, UValueGetterLibrary::GetSimpleLeftVector());

	SelfUpPosition = NewObject<UPositionClaimer>(this, TEXT("SelfUpPosition"));
	SelfUpPosition->Set(NAME_None, EPositionGetterType::SelfPosition, EPositionSpaceType::Self, UValueGetterLibrary::GetSimpleUpVector());

	SelfDownPosition = NewObject<UPositionClaimer>(this, TEXT("SelfDownPosition"));
	SelfDownPosition->Set(NAME_None, EPositionGetterType::SelfPosition, EPositionSpaceType::Self, UValueGetterLibrary::GetSimpleDownVector());
}