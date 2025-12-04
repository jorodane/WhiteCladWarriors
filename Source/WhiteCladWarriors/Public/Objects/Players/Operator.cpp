//// Fill out your copyright notice in the Description page of Project Settings.

#include "Objects/Players/Operator.h"
#include "Objects/Players/AreaSelector.h"
#include "Interfaces/Selectable.h"
#include "Actions/UnitActionComponent.h"
#include "Settings/MapSetting.h"
#include "Settings/ActionSetting.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/CameraComponent.h"

bool AOperator::IsVisibleOnCamera(FMatrix Matrix, AActor* Target)
{
	if(!IsValid(Target)) return false;
	FVector4 ActorLocationOnCamera = Matrix.TransformPosition(Target->GetActorLocation());

	return	UKismetMathLibrary::InRange_FloatFloat((ActorLocationOnCamera.X / ActorLocationOnCamera.W), -1.0, 1.0) &&
			UKismetMathLibrary::InRange_FloatFloat((ActorLocationOnCamera.Y / ActorLocationOnCamera.W), -1.0, 1.0);
}

void AOperator::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (APlayerController* AsPlayerController = Cast<APlayerController>(GetController()))
	{
		FHitResult ClickAreaHitResult;
		FHitResult SelectHitResult;
		bool isClickAreaHit = AsPlayerController->GetHitResultUnderCursorByChannel(ClickAreaChannel, false, ClickAreaHitResult);
		bool isSelectHit = AsPlayerController->GetHitResultUnderCursorByChannel(SelectChannel, false, SelectHitResult);

		if (isClickAreaHit) MouseTerrainPosition = ClickAreaHitResult.Location;
		AActor* CurrentSelectHitActor = isSelectHit ? SelectHitResult.GetActor() : nullptr;
		if (CurrentSelectHitActor != MouseHitActor)
		{
			if (IsValid(MouseHitActor)) ISelectable::Execute_MouseHoverEnd(MouseHitActor);
			MouseHitActor = CurrentSelectHitActor;
			if (IsValid(MouseHitActor)) ISelectable::Execute_MouseHoverBegin(MouseHitActor);
		}
		MouseHitActor = SelectHitResult.GetActor();
	}
}

void AOperator::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void AOperator::UnPossessed()
{
	Super::UnPossessed();
	DeselectActors();
	if (IsValid(MouseHitActor)) ISelectable::Execute_MouseHoverEnd(MouseHitActor);
	MouseHitActor = nullptr;
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

TArray<FActionTargetContainer> AOperator::GetAvailableActionList()
{
	TArray< FActionTargetContainer> Result;

	for (TPair<FName, FActionTargetContainer> CurrentContainer : AvailableActions) Result.Add(CurrentContainer.Value);
	//Result.Sort();

	return Result;
}

TArray<AActor*> AOperator::GetActorsInArea_Implementation(bool& bIsAllSame, bool& bIsSingleSelected)
{
	if(IsValid(DragAreaActor)) return DragAreaActor->GetActorsInArea(this, bIsAllSame, bIsSingleSelected);
	return TArray<AActor*>();
}

TArray<AActor*> AOperator::GetVisibleSameClasses_Implementation(TSubclassOf<AActor> Template)
{
	TArray<AActor*> Result;
	if(!IsValid(Template)) return Result;

	FMinimalViewInfo CurrentView;
	FMatrix ViewMatrix;
	FMatrix ProjectionMatrix;
	FMatrix ViewProjectionCurrentMatrix;
	SelectorCamera->GetCameraView(0.0f, CurrentView);
	UGameplayStatics::CalculateViewProjectionMatricesFromMinimalView(CurrentView, TOptional<FMatrix>(), ViewMatrix, ProjectionMatrix, ViewProjectionCurrentMatrix);
	Result = GetOwnActorsOfClass(Template).FilterByPredicate([&](AActor* CurrentActor)->bool { return IsVisibleOnCamera(ViewProjectionCurrentMatrix, CurrentActor); });

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
		DragAreaActor->SetActorScale3D(FVector(10000.0f, Half.Y, Half.X));
		DragAreaActor->SetActorHiddenInGame(false);
	}
}
void AOperator::SelectToggle_Implementation(AActor* Target)
{
	if (!IsValid(Target)) return;
	if (SelectedActors.Contains(Target)) DeselectActor(Target);
	else SelectActor(Target, true);
}
void AOperator::SelectActorWithoutNotify_Implementation(AActor* Target, bool bIsSingleSelection)
{
	if (!IsValid(Target)) return;
	if (ISelectable::Execute_IsSelectable(Target, this))
	{
		SelectedActors.AddUnique(Target);
		ISelectable::Execute_Select(Target, this, bIsSingleSelection);
		ActorAddToActionList(Target);
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
	if (!IsValid(Target)) return;
	if (SelectedActors.Remove(Target) > 0)
	{
		ISelectable::Execute_Deselect(Target);
		ActorRemoveFromActionList(Target);
	}
}
void AOperator::DeselectActor(AActor* Target)
{
	DeselectActorWithoutNotify(Target);
	OnSelectedChanged.Broadcast(SelectedActors);
}

void AOperator::DeselectActors_Implementation()
{
	for (AActor* CurrentTarget : SelectedActors) ISelectable::Execute_Deselect(CurrentTarget);
	SelectedActors.Empty();
	OnSelectedChanged.Broadcast(SelectedActors);
}

void AOperator::ComponentAddToActionList(UUnitActionComponent* Target)
{
	for (const FName& CurrentActionName : Target->ActionList)
	{
		FActionTargetContainer* CurrentContainer = AvailableActions.Find(CurrentActionName);

		if (CurrentContainer == nullptr)
		{
			CurrentContainer = &AvailableActions.Add(CurrentActionName);
			CurrentContainer->Action = UActionSetting::GetAction(CurrentActionName);
		}
		CurrentContainer->Components.AddUnique(Target);
	}
}

void AOperator::ComponentRemoveFromActionList(UUnitActionComponent* Target)
{
	for (const FName& CurrentActionName : Target->ActionList)
	{
		if (FActionTargetContainer* CurrentContainer = AvailableActions.Find(CurrentActionName))
		{
			CurrentContainer->Components.Remove(Target);
			if (CurrentContainer->Components.Num() == 0) AvailableActions.Remove(CurrentActionName);
		}
	}
}

void AOperator::ActorAddToActionList(AActor* Target)
{
	for (UActorComponent* CurrentComponent : Target->GetComponents())
	{
		if (UUnitActionComponent* AsActionComponent = Cast<UUnitActionComponent>(CurrentComponent))
		{
			ComponentAddToActionList(AsActionComponent);
		}
	}
}

void AOperator::ActorRemoveFromActionList(AActor* Target)
{
	for (UActorComponent* CurrentComponent : Target->GetComponents())
	{
		if (UUnitActionComponent* AsActionComponent = Cast<UUnitActionComponent>(CurrentComponent))
		{
			ComponentRemoveFromActionList(AsActionComponent);
		}
	}
}

void AOperator::OnPlayerConnected_Implementation(AIngameController* NewPlayer)
{
	if (PlayerController) OnPlayerDisconnected(PlayerController);
	
	PlayerController = NewPlayer;
}

void AOperator::OnPlayerDisconnected_Implementation(AIngameController* OldPlayer)
{
	PlayerController = nullptr;
}

