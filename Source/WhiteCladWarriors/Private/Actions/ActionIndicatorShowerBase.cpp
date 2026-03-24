// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/ActionIndicatorShowerBase.h"

void AActionIndicatorShowerBase::SetMiddleLocation_Implementation(FVector A, FVector B)
{
	SetActorLocation(FMath::Lerp(A, B, LocationOffset));
}

void AActionIndicatorShowerBase::SetStartLocation_Implementation(FVector NewLocation)
{
	SetActorLocation(NewLocation);
}

void AActionIndicatorShowerBase::SetDirection_Implementation(FVector NewDirection)
{
	SetRotation(NewDirection.GetSafeNormal2D().Rotation());
}

void AActionIndicatorShowerBase::SetRotation_Implementation(FRotator NewRotator)
{
	SetActorRotation(NewRotator);
}

void AActionIndicatorShowerBase::SetSize_Implementation(FVector NewSize)
{
	SetActorScale3D(NewSize * SizeMultiplier);
}

void AActionIndicatorShowerBase::SetWidth_Implementation(float NewWidth)
{
	FVector NewSize = GetActorScale3D();
	NewSize.Z = NewWidth * SizeMultiplier.Z;
	SetActorScale3D(NewSize);
}

void AActionIndicatorShowerBase::SetLength_Implementation(float NewLength)
{
	FVector NewSize = GetActorScale3D();
	NewSize.Y = NewLength * SizeMultiplier.Y;
	SetActorScale3D(NewSize);
}

void AActionIndicatorShowerBase::SetLengthWidth_Implementation(float NewLength, float NewWidth)
{
	FVector NewSize = GetActorScale3D();
	NewSize.Y = NewLength * SizeMultiplier.Y;
	NewSize.Z = NewWidth * SizeMultiplier.Z;
	SetActorScale3D(NewSize);
}

void AActionIndicatorShowerBase::DrawLineWithRotation_Implementation(FVector StartPosition, FVector EndPosition, float NewWidth, FRotator NewRotation)
{
	SetLengthWidth(FVector::Dist2D(StartPosition, EndPosition), NewWidth);
	SetRotation(NewRotation);
	SetMiddleLocation(StartPosition, EndPosition);
}

void AActionIndicatorShowerBase::DrawFromRotation_Implementation(FVector StartPosition, float NewLength, float NewWidth, FRotator NewRotation)
{
	SetStartLocation(StartPosition);
	SetLengthWidth(NewLength, NewWidth);
	SetRotation(FRotator(0.0, NewRotation.Yaw, 0.0));
}

void AActionIndicatorShowerBase::DrawFromDirection_Implementation(FVector StartPosition, FVector NewDirection, float NewLength, float NewWidth)
{
	SetStartLocation(StartPosition);
	SetLengthWidth(NewLength, NewWidth);
	SetRotation(FRotator(0.0, FMath::RadiansToDegrees(FMath::Atan2(NewDirection.Y, NewDirection.X)), 0.0));
}

void AActionIndicatorShowerBase::DrawLine_Implementation(FVector StartPosition, FVector EndPosition, float NewWidth)
{
	DrawLineWithRotation(StartPosition, EndPosition, NewWidth, (EndPosition - StartPosition).GetSafeNormal2D().Rotation());
}



