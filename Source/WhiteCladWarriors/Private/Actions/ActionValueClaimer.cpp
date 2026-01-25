// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/ActionValueClaimer.h"
#include "Actions/ActionExecutor.h"
#include "Actions/UnitActionComponent.h"
#include "Objects/Selectables/Units/UnitBase.h"
#include "Objects/Players/Operator.h"

FVector UPositionClaimer::GetPosition(const UActionExecutor* Executor, const UUnitActionComponent* Component, const int ID) const
{
	FVector Result = FVector::ZeroVector;

	if (IsValid(Executor) && IsValid(Component)) switch (PositionType)
	{
	case EPositionGetterType::SavedPosition:
		Result = Executor->GetSavedPosition(PositionTag, Component, ID);
	break;
	case EPositionGetterType::SelfPosition:
	{
		AUnitBase* ResultUnit;
		if (Component->TryGetOwnerUnit(ResultUnit)) Result = ResultUnit->GetActorLocation();
	}
	break;
	case EPositionGetterType::ActorPosition:
	{
		AActor* ResultActor = Executor->GetSavedActor(PositionTag, Component, ID);
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
FVector UDirectionClaimer::GetPosition(const UPositionClaimer* Claimer, const UActionExecutor* Executor, const UUnitActionComponent* Component, const int ID) const
{
	if (!IsValid(Executor) || !IsValid(Component)) return FVector::ZeroVector;
	if (!IsValid(Claimer)) if (AUnitBase* Unit = Component->GetOwnerUnit()) Unit->GetActorLocation(); else return FVector::ZeroVector;
	return Claimer->GetPosition(Executor, Component, ID);
}

FVector UDirectionClaimer::GetDirection(const UActionExecutor* Executor, UUnitActionComponent* Component, const int ID) const
{
	FVector Result = GetOriginDirection(Executor,Component,ID);
	if (IsValid(AngleShift))
	{
		FRotator Rotator(0.0, AngleShift->GetValue(), 0.0);
		Result = Rotator.RotateVector(Result);
	}
	return Result;
}

FVector UDirectionClaimer_ToPosition::GetOriginDirection(const UActionExecutor* Executor, UUnitActionComponent* Component, const int ID) const
{
	if (!IsValid(Executor)) return FVector::ZeroVector;
	else return (GetEndPosition(Executor, Component, ID) - GetStartPosition(Executor, Component, ID)).GetSafeNormal();
}
FVector UDirectionClaimer_SavedDirection::GetOriginDirection(const UActionExecutor* Executor, UUnitActionComponent* Component, const int ID) const
{
	if (IsValid(Executor) && DirectionTag.IsNone())return Executor->GetSavedDirection(DirectionTag, Component, ID);
	else return FVector::ZeroVector;
}

AActor* UActorClaimer::GetActor(const UActionExecutor* Executor, const UUnitActionComponent* Component, const int ID) const
{
	if(!IsValid(Executor)) return nullptr;
	return Executor->GetSavedActor(ActorTag, Component, ID);
}

TArray<AActor*> UActorArrayClaimer::GetActorArray(const UActionExecutor* Executor, const UUnitActionComponent* Component, const int ID) const
{
	if (!IsValid(Executor)) return TArray<AActor*>();
	return Executor->GetSavedActorArray(ActorArrayTag, Component, ID);
}

