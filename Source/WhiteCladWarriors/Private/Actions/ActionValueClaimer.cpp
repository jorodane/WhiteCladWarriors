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
	if (IsValid(Executor) && !DirectionTag.IsNone())return Executor->GetSavedDirection(DirectionTag, Component, ID);
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