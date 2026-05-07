//// Fill out your copyright notice in the Description page of Project Settings.

#include "Objects/Players/Operator.h"
#include "Objects/Players/AreaSelector.h"
#include "Objects/Players/InGameController.h"
#include "Objects/Selectables/Components/HeroMainComponent.h"
#include "Objects/Selectables/Components/UnitActionComponent.h"
#include "Actions/ActionBase.h"
#include "Actions/Executables/ActionExecutor.h"
#include "Actions/Executables/ActionSelectorNode.h"
#include "Actions/Indicators/ActionIndicatorBase.h"
#include "Interfaces/Selectable.h"
#include "Camera/CameraComponent.h"
#include "Settings/MapSetting.h"
#include "Settings/ActionSetting.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Math/Matrix.h"
#include "Misc/Optional.h"

TObjectPtr<AOperator> AOperator::LocalOperator = nullptr;
FInputClaim FInputClaim::Claim_None;
FInputPackage FInputPackage::Input_None;

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

	if (AController* CurrentController = GetController())
	{
		if (CurrentController->IsLocalPlayerController())
		{
			CameraMoveToFocusActor();
			UpdateInputPackage();
		}
	}
}

void AOperator::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (NewController->IsLocalPlayerController()) LocalOperator = this;
}

void AOperator::UnPossessed()
{
	Super::UnPossessed();
	DeselectActors();
	if (IsValid(CurrentInputPackage.MouseHitActor)) ISelectable::Execute_MouseHoverEnd(CurrentInputPackage.MouseHitActor);
	CurrentInputPackage.MouseHitActor = nullptr;
	if (LocalOperator == this) LocalOperator = nullptr;
}

void AOperator::OnLeftClick_Implementation(bool bIsMapClick, bool bIsAdditive, bool bIsSelectAll, bool bIsClickForced)
{
	bool bIsClick = bIsClickForced || (CurrentInputPackage.DragStartPosition - CurrentInputPackage.MouseTerrainPosition).SquaredLength() < CLICK_CHECK_SQUARE_DISTANCE;
	if(!bIsClick) CurrentInputPackage.MouseClickActor = nullptr;
	if (IsInputClaimed())
	{
		if (bIsReservationMode) 
		{
			ReservationAction(CurrentInputClaim.TargetActionCursor.CurrentAction, CurrentInputPackage.SelectedActors, CurrentInputPackage, true);
			ForceRemoveInputClaim();
		}
		else
		{
			if (!IsValid(CurrentInputClaim.TargetActionCursor.CurrentExecutor))
			{
				CommandAction(CurrentInputClaim.TargetActionCursor.CurrentAction, CurrentInputClaim.TargetComponentArray, true);
				//CurrentInputClaim.TargetExecutor = CurrentInputClaim.TargetAction->ExecuteActionWithInput(this, CurrentInputClaim.TargetComponentArray, GetInputPackage());
				ForceRemoveInputClaim();
			}
			else
			{
				TArray<FActionCursorFinder> FinderArray;
				for (UUnitActionComponent* CurrentComponent : CurrentInputClaim.TargetComponentArray)
				{
					FActionCursorFinder NewFinder(CurrentInputClaim.TargetActionCursor);
					NewFinder.CurrentComponent = CurrentComponent;
					FinderArray.Add(NewFinder);
					if (UUnitMainComponent* AsUnit = CurrentComponent->GetOwnerUnit()) AsUnit->ReservationClear();
				}
				bool bIsInputComplete =  CurrentInputClaim.TargetActionCursor.CurrentExecutor->SetInputArray(FinderArray, CurrentInputClaim.TargetNode, CurrentInputPackage);
				if (bIsInputComplete) ForceRemoveInputClaim();
			}
		}
	}
	else if(bIsMapClick)
	{
		CameraMoveTo(CurrentInputPackage.MouseTerrainPosition);
		SetFollowingHero(false);
	}
	else
	{
		TArray<AActor*> OutResultArray;
		AActor* OutResultSingle;
		bool OutAllSame, OutOnlySingle;
		double CurrentTime = GetWorld()->GetTimeSeconds();
		bool bIsDoubleClick = IsValid(CurrentInputPackage.MouseHitActor) && CurrentInputPackage.MouseHitActor == CurrentInputPackage.MouseClickActor && (CurrentTime - LastLeftClickTime) < DOUBLE_CLICK_DELAY;
		LastLeftClickTime = CurrentTime;
		if (GetFocusActors(bIsClick, bIsDoubleClick, bIsSelectAllMode, OutResultArray, OutResultSingle, OutAllSame, OutOnlySingle))
		{
			if (bIsClick && bIsAdditive && !(bIsSelectAll || bIsDoubleClick))
			{
				SelectToggle(OutResultSingle);
			}
			else
			{
				SelectActors(OutResultArray, OutOnlySingle, bIsAdditive);
			}
		}
		else if (IsValid(CurrentInputPackage.MouseClickActor))
		{
			SelectActor(CurrentInputPackage.MouseClickActor, true);
		};
		//else if (!bIsAdditive)
		//{
		//	DeselectActors(); //Remove All On Ground Click
		//};

		//if (CurrentInputPackage.SelectedActors.IsEmpty() && IsValid(HeroActor))
		//{
		//	SelectActor(HeroActor, true);
		//}
	}
	if (IsValid(PlayerController))
	{
		PlayerController->SetCursor(GetCursorType());
	}
	CurrentInputPackage.MouseClickActor = nullptr;
}


void AOperator::OnRightClick_Implementation(bool bIsMapClick)
{
	if (IsInputClaimed()) CancelInputClaim();
	else SimpleAction(CurrentInputPackage);
	if (IsValid(PlayerController))
	{
		PlayerController->SetCursor(GetCursorType());
	}
}

void AOperator::OnMapClick_Implementation(bool bIsDown, bool bIsRightClick, FVector ClickLocation)
{
	CurrentInputPackage.MouseHitActor = CurrentInputPackage.MouseClickActor = nullptr;
	if (bIsDown)
	{
		CurrentInputPackage.DragStartPosition = ClickLocation;
	}
	else 
	{
		CurrentInputPackage.MouseTerrainPosition = ClickLocation;
		if (bIsRightClick) OnRightClick(true);
		else OnLeftClick(true, bIsAdditiveMode, bIsSelectAllMode, true);
	}
}

void AOperator::OnMapDrag_Implementation(bool bIsRightClick, FVector ClickLocation)
{
	if (!IsInputClaimed())
	{
		CameraMoveTo(ClickLocation);
		SetFollowingHero(false);
	}
}

void AOperator::OnUpdateInput_Implementation()
{
	HideDragArea();
	OnInputClaimChanged.Broadcast(CurrentInputClaim, IsValid(CurrentInputClaim.TargetNode), false);
	OnUpdateCursor();
}

void AOperator::OnUpdateCursor_Implementation()
{
	if (IsValid(PlayerController))
	{
		PlayerController->SetCursor(GetCursorType());
	}
}


EInputMouseCursorType AOperator::GetCursorType_Implementation()
{
	EInputMouseCursorType ResultCursor = EInputMouseCursorType::Default;
	
	if (CurrentInputClaim.TargetMouseCursorType == EInputMouseCursorType::Default)
	{
		if (IsValid(HoveredOnWidgetObject))
		{
			ResultCursor = EInputMouseCursorType::Selectable;
		}
		else if (IsValid(HoveredOnWorldObject))
		{
			if (AActor* WorldActor = Cast<AActor>(HoveredOnWorldObject))
			{
				UUnitMainComponent* WorldUnit;
				TArray<UUnitActionComponent*> ResultComponents;
				if (AActionBase* WorldAction = GetSimpleActionFromActor(CurrentInputPackage, WorldActor, WorldUnit, ResultComponents))
				{
					if (UActionSelectorNode* WorldSelector = WorldAction->RootNodeAsSelector())
					{
						FInputClaim WorldSimpleInputClaim = WorldSelector->GetInputClaim(ResultComponents, WorldAction, nullptr);

						ResultCursor = WorldSimpleInputClaim.TargetMouseCursorType;
					}
				}
			}
		}
		else
		{
			ResultCursor = GetCursorScrollType();
		}
	}
	else
	{
		ResultCursor = CurrentInputClaim.TargetMouseCursorType;
	}

	return ResultCursor;
}

EInputMouseCursorType AOperator::GetCursorScrollType_Implementation()
{
	EInputMouseCursorType ResultCursor = EInputMouseCursorType::Default;
	if (!MouseScrolling.IsZero())
	{
		if (MouseScrolling.X > 0)
		{
			ResultCursor = EInputMouseCursorType::Right;
		}
		else if (MouseScrolling.X < 0)
		{
			ResultCursor = EInputMouseCursorType::Left;
		}
		else if (MouseScrolling.Y > 0)
		{
			ResultCursor = EInputMouseCursorType::Up;
		}
		else
		{
			ResultCursor = EInputMouseCursorType::Down;
		}
	}
	return ResultCursor;
}

void AOperator::ClaimInput(const FInputClaim& ClaimInfo)
{
	if (IsValid(CurrentInputClaim.TargetNode) && !CurrentInputClaim.TargetNode->bIsCancelable) return;
	CurrentInputClaim.BroadcastEnd();
	CurrentInputClaim = ClaimInfo;
	CurrentInputClaim.BroadcastStart();
	OnUpdateInput();
}

void AOperator::ForceRemoveInputClaim()
{
	CurrentInputClaim.BroadcastEnd();
	CurrentInputClaim.Clear();
	OnUpdateInput();
}

void AOperator::CancelInputClaim()
{
	if (!IsInputClaimed() || CurrentInputClaim.TargetNode == nullptr) return;
	if (!CurrentInputClaim.TargetComponentArray.IsEmpty())
	{
		for (auto CurrentComponent : CurrentInputClaim.TargetComponentArray)
		{
			CurrentInputClaim.TargetNode->CancelInput(CurrentInputClaim.TargetActionCursor);
		}
	}

	ForceRemoveInputClaim();
}

bool AOperator::IsInputClaimed() { return IsValid(CurrentInputClaim.TargetNode); }

void AOperator::UpdateInputPackage()
{
	if (APlayerController* AsPlayerController = Cast<APlayerController>(GetController()))
	{
		FHitResult SelectHitResult;
		bool isSelectHit = AsPlayerController->GetHitResultUnderCursorByChannel(SelectChannel, false, SelectHitResult);

		if (IsValid(PlayerController))
		{
			CurrentInputPackage.MouseTerrainPosition = PlayerController->GetTerrainPosition(GetActorLocation().Z);
		}

		AActor* CurrentSelectHitActor = isSelectHit ? SelectHitResult.GetActor() : nullptr;
		if (CurrentSelectHitActor != CurrentInputPackage.MouseHitActor)
		{
			CurrentInputPackage.MouseHitActor = CurrentSelectHitActor;
			SetHoveredWorldObject(CurrentSelectHitActor);
		}
		CurrentInputPackage.MouseHitActor = SelectHitResult.GetActor();
	}
}

void AOperator::CameraMove(FVector Direction, float Multiplier)
{
	Multiplier *= CameraLength / DEFAULT_CAMERALENGTH;
	Direction.Normalize();
	FVector Result = GetActorLocation();
	Result.X += Direction.Y * Multiplier;
	Result.Y += Direction.X * Multiplier;
	CameraMoveTo(Result);
};

void AOperator::CameraMoveTo(FVector Position)
{
	FVector2D Limit = AMapSetting::GetCurrentMapHalfSize();
	Position.X = FMath::Clamp(Position.X, -Limit.X, Limit.X);
	Position.Y = FMath::Clamp(Position.Y, -Limit.Y, Limit.Y);
	Position.Z = 0;
	SetActorLocation(Position);
}

void AOperator::CameraMoveToFocusActor()
{
	if (IsValid(FocusActor))
	{
		CameraMoveTo(FocusActor->GetActorLocation());
	}
}


void AOperator::CameraZoom_Implementation(float Value, float Min, float Max, float Multiplier)
{
	SetCameraLength(FMath::Clamp(CameraLength + (Value * Multiplier), Min, Max));
}

void AOperator::SetCameraLength_Implementation(float Value)
{
	CameraLength = Value;
}

void AOperator::ResetCameraRotation_Implementation()
{
	SetActorRotation(FQuat::Identity);
}

void AOperator::ResetCameraLength_Implementation()
{
	SetCameraLength(DEFAULT_CAMERALENGTH);
}

void AOperator::EdgeScroll(FVector2D MousePosition, FVector2D ViewportSize, float Multiplier)
{
	if (!FPlatformApplicationMisc::IsThisApplicationForeground()) return;

	FVector2D MouseFromEdge = ViewportSize - MousePosition;
	FVector lastScrolling = MouseScrolling;
	MouseScrolling = FVector::ZeroVector;

	if (MouseFromEdge.X >= 0 && MouseFromEdge.X <= CameraMovePaddingSize) MouseScrolling.X += 1.0f;
	if (MousePosition.X >= 0 && MousePosition.X <= CameraMovePaddingSize) MouseScrolling.X -= 1.0f;
	if (MousePosition.Y >= 0 && MousePosition.Y <= CameraMovePaddingSize) MouseScrolling.Y += 1.0f;
	if (MouseFromEdge.Y >= 0 && MouseFromEdge.Y <= CameraMovePaddingSize) MouseScrolling.Y -= 1.0f;

	MouseScrolling = GetActorTransform().InverseTransformVector(MouseScrolling);

	CameraMove(MouseScrolling.GetSafeNormal2D(), Multiplier);

	if (lastScrolling != MouseScrolling) OnUpdateCursor();
};

void AOperator::SetFocusActor(AActor* Target)
{
	if (IsValid(Target))
	{
		MouseScrolling = FVector::ZeroVector;
		OnUpdateCursor();
		FocusActor = Target;
	}
	else RemoveFocusActor();
}

bool AOperator::IsFocusHero() 
{ 
	return FocusActor == HeroActor; 
}

bool AOperator::IsSingleSelected()
{
	return CurrentInputPackage.SelectedActors.Num() == 1;
}

bool AOperator::IsOnlySelectActor(AActor* Target)
{
	return IsSingleSelected() && CurrentInputPackage.SelectedActors[0] == Target;
}

bool AOperator::IsOnlySelectHero()
{
	return IsOnlySelectActor(HeroActor);
}

void AOperator::SetHoveredWorldObject_Implementation(UObject* NewObject)
{
	if (IsValid(HoveredOnWorldObject) && HoveredOnWorldObject->GetClass()->ImplementsInterface(USelectable::StaticClass())) ISelectable::Execute_MouseHoverEnd(HoveredOnWorldObject);
	HoveredOnWorldObject = NewObject;
	if (!IsValid(HoveredOnWidgetObject)) OnHoverChanged.Broadcast(HoveredOnWorldObject);
	if (IsValid(HoveredOnWorldObject) && HoveredOnWorldObject->GetClass()->ImplementsInterface(USelectable::StaticClass()))
	{
		ISelectable::Execute_MouseHoverBegin(HoveredOnWorldObject);
	}
	OnUpdateCursor();
}

void AOperator::SetHoveredWidgetObject_Implementation(UObject* NewObject)
{
	if (IsValid(HoveredOnWidgetObject) && HoveredOnWidgetObject->GetClass()->ImplementsInterface(USelectable::StaticClass())) ISelectable::Execute_MouseHoverEnd(HoveredOnWidgetObject);
	HoveredOnWidgetObject = NewObject;
	if (IsValid(HoveredOnWidgetObject))
	{
		OnHoverChanged.Broadcast(HoveredOnWidgetObject);
		if(HoveredOnWidgetObject->GetClass()->ImplementsInterface(USelectable::StaticClass())) ISelectable::Execute_MouseHoverBegin(HoveredOnWidgetObject);
	}
	else if(IsValid(HoveredOnWorldObject))
	{
		OnHoverChanged.Broadcast(HoveredOnWorldObject);
	}
	else
	{
		OnHoverChanged.Broadcast(nullptr);
	}
	OnUpdateCursor();
}

void AOperator::RemoveFocusActor()
{
	FocusActor = nullptr;
}

void AOperator::SetToggleFocusActor(AActor* Target)
{
	if (Target == FocusActor) RemoveFocusActor();
	else SetFocusActor(Target);
}

void AOperator::CommandAction(AActionBase* TargetAction, const TArray<UUnitActionComponent*>& TargetComponent, bool bIsStartImmediately)
{
	if(!IsValid(TargetAction)) return;


	FInputClaim ResultInput;
	TArray<bool> ComponentResult;
	EInputType TypeResult;
	FText ReasonResult;

	if (TargetAction->IsNeedInputForStart(ResultInput, TargetComponent))
	{
		if (bIsStartImmediately)
		{
			bool CheckResult = TargetAction->IsValidInputForStart(CurrentInputPackage, this, TargetComponent, ComponentResult, TypeResult, ReasonResult);
			if (CheckResult)
			{
				TargetAction->ExecuteActionWithInput(this, TargetComponent, CurrentInputPackage);
				for (UUnitActionComponent* CurrentComponent : TargetComponent) if (UUnitMainComponent* AsUnit = CurrentComponent->GetOwnerUnit()) AsUnit->ReservationClear();
			}
		}
		else
		{
			ClaimInput(ResultInput);
			return;
		}
	}
	else
	{
		TargetAction->ExecuteAction(this, TargetComponent);
		for (UUnitActionComponent* CurrentComponent : TargetComponent) if (UUnitMainComponent* AsUnit = CurrentComponent->GetOwnerUnit()) AsUnit->ReservationClear();
	}
}

void AOperator::CommandActionForSelectedComponents(AActionBase* TargetAction, bool bIsStartImmediately)
{
	CommandAction(TargetAction, GetAvailableComponentList(TargetAction), bIsStartImmediately);
}

TArray<FActionTargetContainer> AOperator::GetAvailableActionList()
{
	TArray<FActionTargetContainer> Result;

	for (const TPair<FName, FActionTargetContainer>& CurrentContainer : AvailableActions) Result.Add(CurrentContainer.Value);
	Result.Sort([](const FActionTargetContainer& Left, const FActionTargetContainer& Right) -> bool { return Left.GetOrder() < Right.GetOrder(); });

	return Result;
}

TArray<UUnitActionComponent*> AOperator::GetAvailableComponentList(AActionBase* WantAction)
{
	TArray<UUnitActionComponent*> Result;
	if(!IsValid(WantAction)) return Result;

	if (FActionTargetContainer* Container = AvailableActions.Find(WantAction->GetActionNameDefine()))
	{
		Result = Container->Components;
	}

	return Result;
}

TArray<FActionTargetContainer> AOperator::GetAvaliableActionFromKey(FKey WantKey) const
{
	TArray<FActionTargetContainer> Result;
	for (const TPair<FName, FActionTargetContainer>& CurrentPair : AvailableActions)
	{
		const FActionTargetContainer& CurrentContainer = CurrentPair.Value;
		
		if (AActionBase* CurrentAction = CurrentContainer.Action)
		{
			if (CurrentAction->GetHotKey() == WantKey) Result.Add(CurrentContainer);
		}
		
	}
	return Result;
}

bool AOperator::IsSmartKey(AActionBase* TargetAction)
{
	//Get SmartKey Option Here
	return TargetAction->GetIsSmartKey();
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

bool AOperator::GetFocusActors_Implementation(bool bIsClick, bool bIsDoubleClick, bool bIsSelectAll, TArray<AActor*>& OutResultArray, AActor*& OutResultSingle, bool& OutAllSame, bool& OutOnlySingle)
{
	if (bIsClick)
	{
		AActor* MainActor = CurrentInputPackage.MouseHitActor;
		if (IsValid(MainActor))
		{
			if (bIsSelectAll || bIsDoubleClick) OutResultArray = GetVisibleSameActors(MainActor);
			else
			{
				OutResultArray.SetNum(1);
				OutResultArray[0] = MainActor;
				OutAllSame = OutOnlySingle = true;
			}
		}
	}
	else OutResultArray = GetActorsInArea(OutAllSame, OutOnlySingle);
	bool Result = OutResultArray.Num() > 0;
	if(Result) OutResultSingle = OutResultArray[0];
	return Result;
}

void AOperator::DrawDragArea_Implementation(FVector Begin, FVector End)
{
	if (IsValid(DragAreaActor))
	{
		FVector Center = (Begin + End) * 0.5f;
		FVector Half = (End - Center);
		double X = Half.ProjectOnToNormal(GetActorRightVector()).Length();
		double Y = Half.ProjectOnToNormal(GetActorForwardVector()).Length();

		DragAreaActor->SetActorLocationAndRotation(Center, FRotator(-90.0, GetActorRotation().Yaw, 0.0));
		DragAreaActor->SetActorScale3D(FVector(10000.0f, X, Y));
		DragAreaActor->SetActorHiddenInGame(false);
	}
}

void AOperator::HideDragArea_Implementation()
{
	if (IsValid(DragAreaActor))
	{
		DragAreaActor->SetActorHiddenInGame(true);
	}
}

bool AOperator::SelectTest_Implementation(AActor* Target, bool bIsSingleSelected)
{
	return IsValid(Target) && Target->GetClass()->ImplementsInterface(USelectable::StaticClass()) && ISelectable::Execute_CheckSelectable(Target, this, bIsSingleSelected);
}

void AOperator::SelectToggle_Implementation(AActor* Target)
{
	if (!IsValid(Target)) return;
	if (CurrentInputPackage.SelectedActors.Num() > 1 && CurrentInputPackage.SelectedActors.Contains(Target)) DeselectActor(Target);
	else SelectActor(Target, false);
}

void AOperator::SelectActorWithoutNotify_Implementation(AActor* Target, bool bIsSingleSelection)
{
	CurrentInputPackage.SelectedActors.AddUnique(Target);
	ISelectable::Execute_Select(Target, this, bIsSingleSelection);
	if (ActorAddToActionList(Target))
	{
		if (UUnitMainComponent* TargetAsUnit = Target->GetComponentByClass<UUnitMainComponent>())
		{
			TargetAsUnit->OnUnitDie.AddUniqueDynamic(this, &AOperator::DeselectUnit);
		}
	}

}
void AOperator::SelectActor(AActor* Target, bool bIsSingleSelection)
{
	if(SelectInvalid(Target, bIsSingleSelection)) return;
	if (bIsSingleSelection) DeselectActorsWithoutNotify();
	SelectActorWithoutNotify(Target, bIsSingleSelection);
	BroadcastSelectChange();
}

void AOperator::SelectActors_Implementation(TArray<AActor*>& Targets, bool bIsSingleSelection, bool bIsAdditionalSelection)
{
	Targets.RemoveAll([this, bIsSingleSelection](AActor* Target){return !SelectTest(Target, bIsSingleSelection);});
	if(Targets.IsEmpty()) return;

	if(!bIsAdditionalSelection) DeselectActors();

	for (AActor* CurrentTarget : Targets)
	{
		SelectActorWithoutNotify(CurrentTarget, bIsSingleSelection);
	};
	BroadcastSelectChange();
}
void AOperator::DeselectActorWithoutNotify_Implementation(AActor* Target)
{
	if (!IsValid(Target)) return;
	if (CurrentInputPackage.SelectedActors.Remove(Target) > 0)
	{
		ISelectable::Execute_Deselect(Target);
		ActorRemoveFromActionList(Target);
	}
}
void AOperator::DeselectActor(AActor* Target)
{
	DeselectActorWithoutNotify(Target);
	if (CurrentInputPackage.SelectedActors.IsEmpty())SelectActorWithoutNotify(HeroActor, true);
	BroadcastSelectChange();
	
}

void AOperator::DeselectUnit_Implementation(UUnitMainComponent* Target)
{
	if (IsValid(Target))
	{
		Target->OnUnitDie.RemoveAll(this);
		DeselectActor(Target->GetOwner());
	}
};

void AOperator::DeselectActors_Implementation()
{
	DeselectActorsWithoutNotify_Implementation();
	BroadcastSelectChange();
}

void AOperator::DeselectActorsWithoutNotify_Implementation()
{
	for (AActor* CurrentTarget : CurrentInputPackage.SelectedActors)
	{
		ISelectable::Execute_Deselect(CurrentTarget);
		ActorRemoveFromActionList(CurrentTarget);
		if (UUnitMainComponent* CurrentUnit = CurrentTarget->GetComponentByClass<UUnitMainComponent>()) CurrentUnit->OnUnitDie.RemoveAll(this);
	}
	CurrentInputPackage.SelectedActors.Empty();
}

void AOperator::BroadcastSelectChange_Implementation()
{
	Algo::Sort(CurrentInputPackage.SelectedActors, [](AActor* Left, AActor* Right) -> bool { return ISelectable::Execute_GetSelectedorder(Left) < ISelectable::Execute_GetSelectedorder(Right);});
	CurrentInputClaim.TargetComponentArray.RemoveAll([&](UUnitActionComponent* Current)->bool 
		{ 
			if (!IsValid(Current)) return true;
			else if (!CurrentInputPackage.SelectedActors.Contains(Current->GetOwner()))
			{
				Current->OnInputEnd(CurrentInputClaim);
				return true;
			}
			return false;
		});
	if (CurrentInputClaim.TargetComponentArray.IsEmpty()) CancelInputClaim();
	OnSelectedChanged.Broadcast(CurrentInputPackage.SelectedActors);
}

bool AOperator::ComponentAddToActionList(UUnitActionComponent* Target)
{
	bool Result = false;
	if(!IsValid(Target)) return Result;
	if (Target->ActionList.IsEmpty()) return Result;

	for (const FName& CurrentActionName : Target->ActionList)
	{
		FActionTargetContainer* CurrentContainer;

		if (FActionTargetContainer* Finder = AvailableActions.Find(CurrentActionName))
		{
			CurrentContainer = Finder;
		}
		else
		{
			CurrentContainer = &AvailableActions.Add(CurrentActionName);
			CurrentContainer->Action = UActionSetting::GetAction(CurrentActionName);
		}

		if(!CurrentContainer->Components.Contains(Target)) 
		{
			CurrentContainer->Components.Add(Target);
			Result = true;
		}
	}
	return Result;
}

void AOperator::ComponentRemoveFromActionList(UUnitActionComponent* Target)
{
	if(!IsValid(Target)) return;
	for (const FName& CurrentActionName : Target->ActionList)
	{
		if (FActionTargetContainer* Finder = AvailableActions.Find(CurrentActionName))
		{
			FActionTargetContainer& CurrentContainer = *Finder;
			CurrentContainer.Components.Remove(Target);
			if (CurrentContainer.Components.Num() == 0)
			{
				AvailableActions.Remove(CurrentActionName);
			}
		}

	}
}

bool AOperator::ActorAddToActionList(AActor* Target)
{
	bool Result = false;
	TArray<UUnitActionComponent*> ActorComponents;
	Target->GetComponents<UUnitActionComponent>(ActorComponents);
	if (ActorComponents.IsEmpty()) return Result;
	for (UUnitActionComponent* CurrentComponent : ActorComponents)
	{
		Result |= ComponentAddToActionList(CurrentComponent);
	}
	return Result;
}

void AOperator::ActorRemoveFromActionList(AActor* Target)
{
	TArray<UUnitActionComponent*> ActorComponents;
	Target->GetComponents<UUnitActionComponent>(ActorComponents);
	if (ActorComponents.IsEmpty()) return;
	for (UUnitActionComponent* CurrentComponent : ActorComponents)
	{
		ComponentRemoveFromActionList(CurrentComponent);
	}
}

AActionBase* AOperator::GetSimpleActionFromActor(const FInputPackage& Input, AActor* Target, UUnitMainComponent*& OutUnit, TArray<UUnitActionComponent*>& OutComponents)
{
	AActionBase* Result = nullptr;
	if (!IsValid(Target)) return Result;
	OutUnit = Target->GetComponentByClass<UUnitMainComponent>();
	return GetSimpleActionFromComponent(Input, OutUnit, OutComponents);
}
AActionBase* AOperator::GetSimpleActionFromComponent(const FInputPackage& Input, UUnitMainComponent* Target, TArray<UUnitActionComponent*>& OutComponents)
{
	AActionBase* Result = nullptr;
	if (!IsValid(Target)) return Result;
	Target->GetSimpleAction(Input, Result, OutComponents);
	return Result;
}


void AOperator::SimpleAction(const FInputPackage& Input)
{
	TMap<AActionBase*, TSet<UUnitActionComponent*>> ExecuteActionComponentMap;
	TMap<AActionBase*, TSet<AActor*>> ExecuteActionActorMap;

	if (!Input.SelectedActors.IsEmpty())
	{
		for (AActor* CurrentActor : Input.SelectedActors)
		{
			if (!IsValid(CurrentActor)) continue;
			UUnitMainComponent* CurrentAsUnit;
			TArray<UUnitActionComponent*> ResultComponents;
			AActionBase* ResultAction = GetSimpleActionFromActor(Input, CurrentActor, CurrentAsUnit, ResultComponents);
			if (!IsValid(ResultAction)) continue;
			TSet<AActor*>& ResultActorList = ExecuteActionActorMap.FindOrAdd(ResultAction);
			ResultActorList.Add(CurrentActor);
			if (!bIsReservationMode)
			{
				if (ResultComponents.Num() == 0) continue;
				TSet<UUnitActionComponent*>& ResultComponentList = ExecuteActionComponentMap.FindOrAdd(ResultAction);
				ResultComponentList.Append(ResultComponents);
			}
		}
	}


	if (bIsReservationMode)
	{
		if (ExecuteActionActorMap.IsEmpty()) return;
		for (auto& CurrentPair : ExecuteActionActorMap)
		{
			AActionBase* CurrentAction = CurrentPair.Key;
			TSet<AActor*>& CurrentList = CurrentPair.Value;
			if (!IsValid(CurrentAction) || CurrentList.Num() == 0) continue;
			const TArray<AActor*> ResultArray = CurrentList.Array();

			ReservationAction(CurrentAction, ResultArray, CurrentInputPackage, true);
		}
	}
	else
	{
		if (ExecuteActionComponentMap.IsEmpty()) return;

		for (auto& CurrentPair : ExecuteActionComponentMap)
		{
			AActionBase* CurrentAction = CurrentPair.Key;
			TSet<UUnitActionComponent*>& CurrentList = CurrentPair.Value;
			if (!IsValid(CurrentAction) || CurrentList.Num() == 0) continue;
			const TArray<UUnitActionComponent*> ResultArray = CurrentList.Array();
			for (UUnitActionComponent* CurrentComponent : ResultArray) if (UUnitMainComponent* AsUnit = CurrentComponent->GetOwnerUnit()) AsUnit->ReservationClear();
			CommandAction(CurrentAction, ResultArray, true);
		}
	}
}

void AOperator::ReservationAction(AActionBase* TargetAction, TArray<AActor*> TargetActors, const FInputPackage& Input, bool bIsStartImmediately)
{
	FActionReservator Reservator(this, TargetAction, Input);

	if (TargetActors.IsEmpty()) return;

	for (AActor* CurrentActor : TargetActors)
	{
		if (!IsValid(CurrentActor)) continue;
		if (UUnitMainComponent* AsUnit = CurrentActor->GetComponentByClass<UUnitMainComponent>())
		{
			TArray<UUnitActionComponent*> TargetComponents;
			FInputClaim ResultInput;

			if (TargetAction->IsNeedInputForStart(ResultInput, TargetComponents))
			{
				if (bIsStartImmediately)
				{
					EInputType TypeResult;
					FText ReasonResult;
					TArray<bool> ComponentResult;
					bool CheckResult = TargetAction->IsValidInputForStart(Input, this, AsUnit->GetComponentsWithAction(TargetAction), ComponentResult, TypeResult, ReasonResult);
					if (CheckResult)
					{
						AsUnit->ReservationEnqueue(Reservator);
					}
				}
				else
				{
					ClaimInput(ResultInput);
					return;
				}
			}
			else
			{
				AsUnit->ReservationEnqueue(Reservator);
			}
		};
	}
}

UHeroMainComponent* AOperator::SpawnHero(FVector Location)
{
	UHeroMainComponent* Result = nullptr;
	if (IsValid(HeroComponent) && IsValid(HeroActor))
	{
		Result = HeroComponent;
		HeroActor->SetActorLocation(Location);
		return Result;
	}

	if (UWorld* CurrentWorld = GetWorld())
	{
		AActor* SpawnedActor = CurrentWorld->SpawnActor(HeroClass, &Location);
		if (SpawnedActor)
		{
			SpawnedActor->SetOwner(this);
			Result = HeroComponent = SpawnedActor->GetComponentByClass<UHeroMainComponent>();
			HeroActor = SpawnedActor;
			IPlayerConnectable::Execute_OnPlayerConnected(HeroComponent, PlayerController);
		}
	}
	else
	{
		Result = nullptr;
	}

	OnHeroChanged.Broadcast(Result);

	return Result;
}

void AOperator::SetFollowingHero(bool Value)
{ 
	if (Value && IsValid(HeroActor))
	{
		DeselectActors();
		SelectActor(HeroActor, true);
		SetFocusActor(HeroActor);
	}
	else
	{
		RemoveFocusActor();
	}
}

void AOperator::ToggleFollowingHero()
{ 
	ResetCameraRotation();
	if (IsFocusHero() && !IsOnlySelectHero())
	{
		DeselectActors();
		SelectActor(HeroActor, true);
	}
	else
	{
		SetToggleFocusActor(HeroActor);
		if (IsFocusHero()) SelectActor(HeroActor, true);
	}
}


void AOperator::OnPlayerConnected_Implementation(AIngameController* NewPlayer)
{
	if (PlayerController) OnPlayerDisconnected(PlayerController);
	
	PlayerController = NewPlayer;
	if (IsValid(HeroActor))
	{
		IPlayerConnectable::Execute_OnPlayerConnected(HeroActor, NewPlayer);
	}
}

void AOperator::OnPlayerDisconnected_Implementation(AIngameController* OldPlayer)
{
	if (IsValid(HeroActor))
	{
		IPlayerConnectable::Execute_OnPlayerConnected(HeroActor, OldPlayer);
	}
	PlayerController = nullptr;
}



TArray<FActionTargetContainer> AOperator::GetAvailableActionListFromActors(const TArray<AActor*>& TargetArray)
{
	TArray<FActionTargetContainer> Result;

	if (TargetArray.IsEmpty()) return Result;

	TMap<FName, FActionTargetContainer> Collector;

	for (AActor* CurrentTarget : TargetArray) AppendAvailableActionFromActor(CurrentTarget, Collector);

	Collector.GenerateValueArray(Result);
	Result.Sort([](const FActionTargetContainer& Left, const FActionTargetContainer& Right) -> bool { return Left.GetOrder() < Right.GetOrder(); });
	return Result;

}

void AOperator::AppendAvailableActionFromComponent(UUnitActionComponent* TargetComponent, TMap<FName, FActionTargetContainer>& OutMap)
{
	if (!IsValid(TargetComponent)) return;

	for (const FName& CurrentActionName : TargetComponent->ActionList)
	{
		if (FActionTargetContainer* Container = OutMap.Find(CurrentActionName))
		{
			if (!Container->Components.Contains(TargetComponent)) Container->Components.Add(TargetComponent);
		}
		else
		{
			AActionBase* NewAction = UActionSetting::GetAction(CurrentActionName);
			OutMap.Add(CurrentActionName, NewAction).Components.Add(TargetComponent);
		}
	}
}

void AOperator::AppendAvailableActionFromActor(AActor* TargetActor, TMap<FName, FActionTargetContainer>& OutMap)
{
	if (!IsValid(TargetActor)) return;
	TArray<UUnitActionComponent*> ActionComponentArray;
	TargetActor->GetComponents<UUnitActionComponent>(ActionComponentArray);
	for (UActorComponent* CurrentComponent : ActionComponentArray)
	{
		AppendAvailableActionFromComponent(Cast<UUnitActionComponent>(CurrentComponent), OutMap);
	}
}