//// Fill out your copyright notice in the Description page of Project Settings.

#include "Objects/Players/Operator.h"
#include "Interfaces/Selectable.h"
#include "Settings/MapSetting.h"

// Sets default values
void AOperator::CameraMove(FVector2D Direction, float Multiplier)
{
	Multiplier *= CameraLength / DEFAULT_CAMERALENGTH;
	FVector2D Limit = AMapSetting::GetCurrentMapHalfSize();
	FVector Result = GetActorLocation();
	Direction.Normalize();
	Result.X = FMath::Clamp(Result.X + (Direction.Y * Multiplier), -Limit.X, Limit.X);
	Result.Y = FMath::Clamp(Result.Y + (Direction.X * Multiplier), -Limit.Y, Limit.Y);
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

TArray<AActor*> AOperator::GetObjectsInArea_Implementation()
{
	return SelectedActors;
}
TArray<AActor*> AOperator::GetVisibleSameObjects_Implementation(AActor* Template)
{
	return SelectedActors;
}
void AOperator::DrawDragArea_Implementation(FVector Begin, FVector End)
{
	if (IsValid(DragAreaActor))
	{
		FVector Center = (Begin + End) * 0.5f;
		FVector Half = (End - Begin).GetAbs() * 0.5f;

		DragAreaActor->SetActorLocation(Center);
		DragAreaActor->SetActorScale3D(FVector(10000.0f, Half.X, Half.Y));
		DragAreaActor->SetActorHiddenInGame(false);
	}
}
void AOperator::SelectToggle_Implementation(AActor* Target)
{
	if (Target == nullptr) return;
	if (SelectedActors.Contains(Target))
	{
		DeselectObject(Target);
	}
	else
	{
		SelectObject(Target, true);
	}
}
void AOperator::SelectObjectWithoutNotify_Implementation(AActor* Target, bool bIsSingleSelection)
{
	if (Target == nullptr) return;
	if (ISelectable::Execute_IsSelectable(Target, this))
	{
		SelectedActors.AddUnique(Target);
		ISelectable::Execute_Select(Target, this, bIsSingleSelection);
	}
}
void AOperator::SelectObject(AActor* Target, bool bIsSingleSelection)
{ 
	SelectObjectWithoutNotify(Target, bIsSingleSelection);
	OnSelectedChanged.Broadcast(SelectedActors); 
}

void AOperator::SelectObjects_Implementation(const TArray<AActor*> Targets, bool bIsSingleSelection)
{
	for (AActor* CurrentTarget : Targets)
	{
		SelectObjectWithoutNotify(CurrentTarget, bIsSingleSelection);
	};
	OnSelectedChanged.Broadcast(SelectedActors);
}
void AOperator::DeselectObjectWithoutNotify_Implementation(AActor* Target)
{
	if (Target == nullptr) return;
	if (SelectedActors.Remove(Target) > 0)
	{
		ISelectable::Execute_Deselect(Target);
	}
}
void AOperator::DeselectObject(AActor* Target) 
{ 
	DeselectObjectWithoutNotify(Target); 
	OnSelectedChanged.Broadcast(SelectedActors); 
}

void AOperator::DeselectObjects_Implementation()
{
	for (AActor* CurrentTarget : SelectedActors)
	{
		DeselectObjectWithoutNotify(CurrentTarget);
	};
	OnSelectedChanged.Broadcast(SelectedActors);
}


