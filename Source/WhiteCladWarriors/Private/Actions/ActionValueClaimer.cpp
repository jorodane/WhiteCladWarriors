// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/ActionValueClaimer.h"
#include "Actions/ActionExecutor.h"
#include "Actions/UnitActionComponent.h"
#include "Objects/Selectables/Units/UnitBase.h"
#include "Objects/Players/Operator.h"

FVector FPositionClaimer::GetPosition(const UActionExecutor* Executor, const UUnitActionComponent* Component, const int ID) const
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
		const FInputPackage& LocalInput = AOperator::GetLocalInputPackage();
		Result = LocalInput.MouseTerrainPosition;
	}
	break;
	case EPositionGetterType::WorldPosition:
	default:
		break;
	}
	if (bIsAdditive) Result += GetAdditivePosition(Component);
	return Result;
}

FVector FPositionClaimer::GetAdditivePosition(const UUnitActionComponent* Component) const
{
	if (AdditiveSpace == EPositionSpaceType::Self && IsValid(Component)) 
	{
		AUnitBase* ResultUnit;
		if (Component->TryGetOwnerUnit(ResultUnit))
		{
			return ResultUnit->GetActorTransform().TransformVector(AdditivePosition);
		}
	}
	return AdditivePosition;
}
FVector FDirectionClaimer::GetPosition(const UActionExecutor* Executor, const UUnitActionComponent* Component, const int ID) const
{
	return From.GetPosition(Executor, Component, ID);
}
FVector FDirectionClaimer::GetDirection(const UActionExecutor* Executor, UUnitActionComponent* Component, const int ID) const
{
	if (!IsValid(Executor)) return FVector::ZeroVector;
	return Executor->GetSavedDirection(DirectionTag, Component, ID);
}

AActor* FActorClaimer::GetActor(const UActionExecutor* Executor, const UUnitActionComponent* Component, const int ID) const
{
	if(!IsValid(Executor)) return nullptr;
	return Executor->GetSavedActor(ActorTag, Component, ID);
}

TArray<AActor*> FActorArrayClaimer::GetActorArray(const UActionExecutor* Executor, const UUnitActionComponent* Component, const int ID) const
{
	if (!IsValid(Executor)) return TArray<AActor*>();
	return Executor->GetSavedActorArray(ActorArrayTag, Component, ID);
}

