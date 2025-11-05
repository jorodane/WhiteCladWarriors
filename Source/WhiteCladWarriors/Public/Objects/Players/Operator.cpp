//// Fill out your copyright notice in the Description page of Project Settings.

#include "Objects/Players/Operator.h"

// Sets default values
void AOperator::CameraMove(FVector2D Direction, float Multiplier)
{
	Multiplier *= CameraLength / DefaultCameraLength;
	FVector Result = GetActorLocation();
	Direction.Normalize();
	Result.X += Direction.Y * Multiplier;
	Result.Y += Direction.X * Multiplier;
	SetActorLocation(Result);
};

void AOperator::CameraZoom_Implementation(float Value, float Min, float Max, float Multiplier)
{
	SetCameraLength(FMath::Clamp(CameraLength + (Value * Multiplier), Min, Max));
}

void AOperator::SetCameraLength_Implementation(float Value)
{
	CameraLength = Value;
}

void AOperator::EdgeScroll(FVector2D MousePosition, FVector2D ViewportSize, float Multiplier)
{
	FVector2D MouseFromEdge = ViewportSize - MousePosition;
	FVector2D Result = FVector2D::ZeroVector;

	if (MousePosition.X >= 0 && MousePosition.X <= CameraMovePaddingSize) Result.X += -1.0f;
	if (MouseFromEdge.X >= 0 && MouseFromEdge.X <= CameraMovePaddingSize) Result.X += 1.0f;
	if (MousePosition.Y >= 0 && MousePosition.Y <= CameraMovePaddingSize) Result.Y += 1.0f;
	if (MouseFromEdge.Y >= 0 && MouseFromEdge.Y <= CameraMovePaddingSize) Result.Y += -1.0f;

	CameraMove(Result, Multiplier);
};

