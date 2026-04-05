// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Selectables/Components/UnitMovementComponent.h"

void UUnitMovementComponent::SetPath(UNavigationPath* NewPath)
{
	CurrentPath = NewPath; 
}

FVector UUnitMovementComponent::GetPathLocationWithPercent(double Percent, FVector CurrentLocation)
{
	return GetPathLocationWithDistance(FMath::Clamp(CurrentPath->GetPathLength() * Percent, 0.0, 1.0), CurrentLocation);
}


FVector UUnitMovementComponent::GetPathLocationWithDistance(double Distance, FVector CurrentLocation)
{
	if (!IsValid(CurrentPath)) return CurrentLocation;
	const TArray<FVector>& Points = CurrentPath->PathPoints;
	int PointsNum = Points.Num();
	if (PointsNum == 0) return CurrentLocation;
	CurrentLocation = Points[0];
	if (PointsNum == 1 || Distance <= 0.0) return CurrentLocation;
	if (Distance >= CurrentPath->GetPathLength())
	{
		CurrentLocation = Points.Last();
		return CurrentLocation;
	}
	double WantLength = Distance;
	for (int i = 1; i < PointsNum; ++i)
	{
		const FVector& PointLocation = Points[i];
		double CurrentLength = FVector::Distance(PointLocation, CurrentLocation);
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