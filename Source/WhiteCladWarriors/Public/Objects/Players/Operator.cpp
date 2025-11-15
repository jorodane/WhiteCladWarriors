//// Fill out your copyright notice in the Description page of Project Settings.

#include "Objects/Players/Operator.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Interfaces/Selectable.h"
#include "Camera/CameraComponent.h"
#include "Settings/MapSetting.h"

const FVector2D AOperator::CameraVisibleRange = FVector2D(-1.2f, 1.2f);

bool AOperator::IsVisibleOnCamera(FMatrix Matrix, AActor* Target)
{
	FVector4 ActorLocationOnCamera = Matrix.TransformPosition(Target->GetActorLocation());
	
	return ActorLocationOnCamera.W > 0.0f 
		&& UKismetMathLibrary::InRange_FloatFloat((ActorLocationOnCamera.X / ActorLocationOnCamera.W), CameraVisibleRange.X, CameraVisibleRange.Y)
		&& UKismetMathLibrary::InRange_FloatFloat((ActorLocationOnCamera.Y / ActorLocationOnCamera.W), CameraVisibleRange.X, CameraVisibleRange.Y)
		&& UKismetMathLibrary::InRange_FloatFloat((ActorLocationOnCamera.Z / ActorLocationOnCamera.W), CameraVisibleRange.X, CameraVisibleRange.Y);
}

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

TArray<AActor*> AOperator::GetActorsInArea_Implementation()
{
	TArray<AActor*> Result;
	return Result;
}

TArray<AActor*> AOperator::GetVisibleSameClasses_Implementation(TSubclassOf<AActor> Template)
{
	TArray<AActor*> Result;
	FMinimalViewInfo CurrentView;
	FMatrix CurrentMatrix;

	SelectorCamera->GetCameraView(0.0f, CurrentView);
	CurrentMatrix = CurrentView.CalculateProjectionMatrix();

	Result = GetOwnActorsOfClass(Template).FilterByPredicate([&](AActor* CurrentActor)->bool 
	{ 
			return IsValid(CurrentActor) && CurrentActor->GetClass() == Template && IsVisibleOnCamera(CurrentMatrix, CurrentActor);
	});

	return Result;
}

TArray<AActor*> AOperator::GetOwnActors_Implementation()
{
	TArray<AActor*> Result;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), Result);
	return Result;
}

TArray<AActor*> AOperator::GetOwnActorsOfClass_Implementation(TSubclassOf<AActor> Template)
{
	TArray<AActor*> Result;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), Template, Result);
	return Result;
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
		DeselectActor(Target);
	}
	else
	{
		SelectActor(Target, true);
	}
}
void AOperator::SelectActorWithoutNotify_Implementation(AActor* Target, bool bIsSingleSelection)
{
	if (Target == nullptr) return;
	if (ISelectable::Execute_IsSelectable(Target, this))
	{
		SelectedActors.AddUnique(Target);
		ISelectable::Execute_Select(Target, this, bIsSingleSelection);
	}
}
void AOperator::SelectActor(AActor* Target, bool bIsSingleSelection)
{ 
	SelectActorWithoutNotify(Target, bIsSingleSelection);
	OnSelectedChanged.Broadcast(SelectedActors); 
}

void AOperator::SelectActors_Implementation(const TArray<AActor*>& Targets, bool bIsSingleSelection)
{
	for (AActor* CurrentTarget : Targets)
	{
		SelectActorWithoutNotify(CurrentTarget, bIsSingleSelection);
	};
	OnSelectedChanged.Broadcast(SelectedActors);
}
void AOperator::DeselectActorWithoutNotify_Implementation(AActor* Target)
{
	if (Target == nullptr) return;
	if (SelectedActors.Remove(Target) > 0)
	{
		ISelectable::Execute_Deselect(Target);
	}
}
void AOperator::DeselectActor(AActor* Target) 
{ 
	DeselectActorWithoutNotify(Target); 
	OnSelectedChanged.Broadcast(SelectedActors); 
}

void AOperator::DeselectActors_Implementation()
{
	for (AActor* CurrentTarget : SelectedActors)
	{
		DeselectActorWithoutNotify(CurrentTarget);
	};
	OnSelectedChanged.Broadcast(SelectedActors);
}


