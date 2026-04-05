// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Selectables/Components/UnitMovementComponent.h"
#include "Objects/Selectables/Units/UnitBase.h"

void UUnitMovementComponent::SetPath(UNavigationPath* NewPath)
{
	if (CurrentPath != NewPath)
	{
		if(IsValid(CurrentPath)) CurrentPath->PathUpdatedNotifier.RemoveAll(this);
		if(IsValid(NewPath))NewPath->PathUpdatedNotifier.AddDynamic(this, &UUnitMovementComponent::OnPathUpdated);
	}
	MovedDistance = 0.0;
	CurrentPath = NewPath; 
	if (!IsValid(CurrentPath))
	{
		PathLength = 0.0;
		DistanceArray.Empty();
		return;
	}
	PathLength = CurrentPath->GetPathLength();
	const TArray<FVector>& Points = CurrentPath->PathPoints;
	int PointsNum = Points.Num();
	if (PointsNum <= 1)
	{	
		DistanceArray.Empty();
		return;
	}

	int DistanceNum = Points.Num() - 1;
	DistanceArray.SetNum(Points.Num() - 1);

	FVector CurrentLocation = Points[0];
	for (int i = 1; i < PointsNum; ++i)
	{
		const FVector& PointLocation = Points[i];
		double CurrentLength = FVector::Distance(PointLocation, CurrentLocation);
		DistanceArray[i - 1] = CurrentLength;
		CurrentLocation = PointLocation;
	}
}

bool UUnitMovementComponent::MoveNextDistance(double WantDistance)
{
	bool Result = false;
	if (!IsValid(CurrentPath)) return Result;
	AUnitBase* Unit = GetOwnerUnit();
	if (!IsValid(Unit)) return Result;
	MovedDistance += WantDistance;
	if (MovedDistance >= PathLength) Result = true;
	const FVector& OriginLocation = Unit->GetActorLocation();
	const FVector& GoalLocation = GetPathLocationWithDistance(MovedDistance, OriginLocation);// +(FVector::UpVector * Unit->GetHalfHeight());
	Unit->SetActorLocation(GoalLocation);

	return Result;
}


FVector UUnitMovementComponent::GetPathLocationWithDistance(double Distance, FVector CurrentLocation)
{
	if (!IsValid(CurrentPath)) return CurrentLocation;
	const TArray<FVector>& Points = CurrentPath->PathPoints;
	int PointsNum = Points.Num();
	if (PointsNum == 0) return CurrentLocation;
	CurrentLocation = Points[0];
	if (PointsNum == 1 || Distance <= 0.0) return CurrentLocation;
	if (Distance >= PathLength)
	{
		CurrentLocation = Points.Last();
		return CurrentLocation;
	}
	double WantLength = Distance;
	for (int i = 0; i < PointsNum - 1; ++i)
	{
		const FVector& PointLocation = Points[i + 1];
		double CurrentLength = DistanceArray[i];
		if (CurrentLength < WantLength)
		{
			WantLength -= CurrentLength;
			CurrentLocation = PointLocation;
		}
		else if (CurrentLength != 0.0)
		{
			CurrentLocation += (PointLocation - CurrentLocation).GetSafeNormal() * WantLength;
			break;
		}
	}

	return CurrentLocation;
}

FVector UUnitMovementComponent::GetPathLocationWithPercent(double Percent, FVector CurrentLocation)
{
	return GetPathLocationWithDistance(FMath::Clamp(Percent, 0.0, 1.0), CurrentLocation);
}

void UUnitMovementComponent::OnPathUpdated(UNavigationPath* AffectedPath, TEnumAsByte<ENavPathEvent::Type> PathEvent)
{
	switch (PathEvent)
	{
		case ENavPathEvent::Type::Cleared:
		case ENavPathEvent::Type::Invalidated:
		case ENavPathEvent::Type::RePathFailed:
			SetPath(nullptr);
			break;
		case ENavPathEvent::Type::NewPath:
		case ENavPathEvent::Type::UpdatedDueToGoalMoved:
		case ENavPathEvent::Type::UpdatedDueToNavigationChanged:
		case ENavPathEvent::Type::MetaPathUpdate:
		case ENavPathEvent::Type::Custom:
			SetPath(AffectedPath);
			break;
	}
}
