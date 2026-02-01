//// Fill out your copyright notice in the Description page of Project Settings.

#include "Objects/Players/Operator.h"
#include "Objects/Players/AreaSelector.h"
#include "Objects/Players/InGameController.h"
#include "Objects/Selectables/Units/UnitBase.h"
#include "Actions/ActionBase.h"
#include "Actions/ActionExecutor.h"
#include "Actions/ActionSelectorNode.h"
#include "Actions/UnitActionComponent.h"
#include "Interfaces/Selectable.h"
#include "Camera/CameraComponent.h"
#include "Settings/MapSetting.h"
#include "Settings/ActionSetting.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Misc/Optional.h"
#include "HAL/PlatformApplicationMisc.h"

TObjectPtr<AOperator> AOperator::LocalOperator = nullptr;
FInputClaim FInputClaim::Claim_None;
FInputPackage FInputPackage::Input_None;

bool UActionTargetContainer::operator < (const UActionTargetContainer& Other) const
{
	return (IsValid(this->Action) ? this->Action->GetUIOrder() : 0) < (IsValid(Other.Action) ? Other.Action->GetUIOrder() : 0);
}

bool UActionTargetContainer::operator > (const UActionTargetContainer& Other) const
{
	return (IsValid(this->Action) ? this->Action->GetUIOrder() : 0) > (IsValid(Other.Action) ? Other.Action->GetUIOrder() : 0);
}


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

		if (isClickAreaHit) CurrentInputPackage.MouseTerrainPosition = ClickAreaHitResult.Location;
		AActor* CurrentSelectHitActor = isSelectHit ? SelectHitResult.GetActor() : nullptr;
		if (CurrentSelectHitActor != CurrentInputPackage.MouseHitActor)
		{
			if (IsValid(CurrentInputPackage.MouseHitActor)) ISelectable::Execute_MouseHoverEnd(CurrentInputPackage.MouseHitActor);
			CurrentInputPackage.MouseHitActor = CurrentSelectHitActor;
			if (IsValid(CurrentInputPackage.MouseHitActor)) ISelectable::Execute_MouseHoverBegin(CurrentInputPackage.MouseHitActor);
		}
		CurrentInputPackage.MouseHitActor = SelectHitResult.GetActor();
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

void AOperator::OnLeftClick_Implementation(bool bIsMapClick, bool bIsAdditive, bool bIsSelectAll)
{
	bool bIsClick = (CurrentInputPackage.DragStartPosition - CurrentInputPackage.MouseTerrainPosition).SquaredLength() < CLICK_CHECK_SQUARE_DISTANCE;
	if (IsInputClaimed())
	{
		if (bIsReservationMode) 
		{
			ReservationAction(CurrentInputClaim.TargetAction, CurrentInputPackage.SelectedActors, CurrentInputPackage);
		}
		else
		{
			if (!IsValid(CurrentInputClaim.TargetExecutor))
			{
				CommandAction(CurrentInputClaim.TargetAction, CurrentInputClaim.TargetComponentArray, true);
				//CurrentInputClaim.TargetExecutor = CurrentInputClaim.TargetAction->ExecuteActionWithInput(this, CurrentInputClaim.TargetComponentArray, GetInputPackage());
				ForceRemoveInputClaim();
			}
			else
			{
				bool bIsInputComplete =  CurrentInputClaim.TargetExecutor->SetInputArray(CurrentInputClaim.TargetComponentArray, CurrentInputClaim.ID, CurrentInputClaim.TargetNode, CurrentInputPackage);
				if (bIsInputComplete) ForceRemoveInputClaim();
				for (UUnitActionComponent* CurrentComponent : CurrentInputClaim.TargetComponentArray) if (AUnitBase* AsUnit = CurrentComponent->GetOwnerUnit()) AsUnit->ReservationClear();
			}
		}
	}
	else if(bIsMapClick)
	{
		CameraMoveTo(CurrentInputPackage.MouseTerrainPosition);
	}
	else
	{
		TArray<AActor*> OutResultArray;
		AActor* OutResultSingle;
		bool OutAllSame, OutOnlySingle;
		double CurrentTime = GetWorld()->GetTimeSeconds();
		bool bIsDoubleClick = IsValid(CurrentInputPackage.MouseHitActor) && CurrentInputPackage.MouseHitActor == CurrentInputPackage.MouseClickActor && (CurrentTime - LastLeftClickTime) < DOUBLE_CLICK_DELAY;
		LastLeftClickTime = CurrentTime;
		CurrentInputPackage.MouseClickActor = CurrentInputPackage.MouseHitActor;
		if (GetFocusActors(bIsClick, bIsDoubleClick, bIsSelectAllMode, OutResultArray, OutResultSingle, OutAllSame, OutOnlySingle))
		{
			if (!bIsAdditive) DeselectActors();
			if(bIsClick && !(bIsSelectAll || bIsDoubleClick))
			{
				SelectToggle(OutResultSingle);
				return;
			}
			SelectActors(OutResultArray, OutOnlySingle);
		}
		//else if(!bIsAdditive) DeselectActors(); Remove All On Ground Click
	}
}


void AOperator::OnRightClick_Implementation(bool bIsMapClick)
{
	if (IsInputClaimed()) CancelInputClaim();
	else SimpleAction(CurrentInputPackage);
}

void AOperator::OnMapClick_Implementation(bool bIsDown, bool bIsRightClick, FVector ClickLocation)
{
	CurrentInputPackage.MouseHitActor = CurrentInputPackage.MouseClickActor = nullptr;
	if(bIsDown) CurrentInputPackage.DragStartPosition = ClickLocation;
	else 
	{
		CurrentInputPackage.MouseTerrainPosition = ClickLocation;
		if (bIsRightClick) OnRightClick(true);
		else OnLeftClick(true, bIsAdditiveMode, bIsSelectAllMode);
	}
}

void AOperator::OnMapDrag_Implementation(bool bIsRightClick, FVector ClickLocation)
{
	if (!IsInputClaimed()) CameraMoveTo(ClickLocation);
}

void AOperator::OnUpdateInput_Implementation()
{
	if (IsValid(PlayerController))PlayerController->SetCursor(CurrentInputClaim.TargetCursor);
	OnInputClaimChanged.Broadcast(CurrentInputClaim);
}

void AOperator::ClaimInput(const FInputClaim& ClaimInfo)
{
	if (IsValid(CurrentInputClaim.TargetNode) && !CurrentInputClaim.TargetNode->bIsCancelable) return;
	CurrentInputClaim = ClaimInfo;
	OnUpdateInput();
}

void AOperator::ForceRemoveInputClaim()
{
	CurrentInputClaim = FInputClaim::Claim_None;
	OnUpdateInput();
}

void AOperator::CancelInputClaim()
{
	if (!IsInputClaimed() || CurrentInputClaim.TargetNode == nullptr) return;
	for (auto CurrentComponent : CurrentInputClaim.TargetComponentArray)
	{
		if (CurrentInputClaim.TargetNode->CancelInput(CurrentInputClaim.TargetExecutor, CurrentComponent, CurrentInputClaim.ID)) ForceRemoveInputClaim();
	}
}

bool AOperator::IsInputClaimed() { return IsValid(CurrentInputClaim.TargetNode); }

void AOperator::CameraMove(FVector2D Direction, float Multiplier)
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
	SetActorLocation(Position);
}

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
	if (!FPlatformApplicationMisc::IsThisApplicationForeground()) return;

	FVector2D MouseFromEdge = ViewportSize - MousePosition;
	FVector2D Result = FVector2D::ZeroVector;

	if (MouseFromEdge.X >= 0 && MouseFromEdge.X <= CameraMovePaddingSize) Result.X += 1.0f;
	if (MousePosition.X >= 0 && MousePosition.X <= CameraMovePaddingSize) Result.X -= 1.0f;
	if (MousePosition.Y >= 0 && MousePosition.Y <= CameraMovePaddingSize) Result.Y += 1.0f;
	if (MouseFromEdge.Y >= 0 && MouseFromEdge.Y <= CameraMovePaddingSize) Result.Y -= 1.0f;

	CameraMove(Result, Multiplier);
};

void AOperator::CommandAction(AActionBase* TargetAction, const TArray<UUnitActionComponent*>& TargetComponent, bool bIsStartImmediately)
{
	if(!IsValid(TargetAction)) return;

	FInputClaim ResultInput;

	if (TargetAction->IsNeedInputForStart(ResultInput, TargetComponent))
	{
		if (bIsStartImmediately)
		{
			TargetAction->ExecuteActionWithInput(this, TargetComponent, CurrentInputPackage);
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
	}
	for (UUnitActionComponent* CurrentComponent : TargetComponent) if (AUnitBase* AsUnit = CurrentComponent->GetOwnerUnit()) AsUnit->ReservationClear();
}

TArray<UActionTargetContainer*> AOperator::GetAvailableActionList()
{
	TArray<UActionTargetContainer*> Result;

	for (TPair<FName, UActionTargetContainer*> CurrentContainer : AvailableActions) Result.Add(CurrentContainer.Value);
	Result.Sort();

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
		FVector Half = (End - Begin).GetAbs() * 0.5f;

		DragAreaActor->SetActorLocation(Center);
		DragAreaActor->SetActorScale3D(FVector(10000.0f, Half.Y, Half.X));
		DragAreaActor->SetActorHiddenInGame(false);
	}
}
void AOperator::SelectToggle_Implementation(AActor* Target)
{
	if (!IsValid(Target)) return;
	if (CurrentInputPackage.SelectedActors.Contains(Target)) DeselectActor(Target);
	else SelectActor(Target, true);
}
void AOperator::SelectActorWithoutNotify_Implementation(AActor* Target, bool bIsSingleSelection)
{
	if (!IsValid(Target)) return;
	if (ISelectable::Execute_IsSelectable(Target, this))
	{
		CurrentInputPackage.SelectedActors.AddUnique(Target);
		ISelectable::Execute_Select(Target, this, bIsSingleSelection);
		ActorAddToActionList(Target);
		if(AUnitBase* TargetAsUnit = Cast<AUnitBase>(Target)) TargetAsUnit->OnUnitDie.AddDynamic(this, &AOperator::DeselectUnit);
	}

}
void AOperator::SelectActor(AActor* Target, bool bIsSingleSelection)
{
	SelectActorWithoutNotify(Target, bIsSingleSelection);
	OnSelectedChanged.Broadcast(CurrentInputPackage.SelectedActors);
}

void AOperator::SelectActors_Implementation(const TArray<AActor*>& Targets, bool bIsSingleSelection)
{
	for (AActor* CurrentTarget : Targets)
	{
		SelectActorWithoutNotify(CurrentTarget, bIsSingleSelection);
	};
	OnSelectedChanged.Broadcast(CurrentInputPackage.SelectedActors);
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
	OnSelectedChanged.Broadcast(CurrentInputPackage.SelectedActors);
}

void AOperator::DeselectUnit_Implementation(AUnitBase* Target) 
{ 
	if(IsValid(Target)) Target->OnUnitDie.RemoveAll(this);
	DeselectActor(Target); 
};

void AOperator::DeselectActors_Implementation()
{
	for (AActor* CurrentTarget : CurrentInputPackage.SelectedActors)
	{
		ISelectable::Execute_Deselect(CurrentTarget);
		ActorRemoveFromActionList(CurrentTarget);
		if (AUnitBase* CurrentUnit = Cast<AUnitBase>(CurrentTarget)) CurrentUnit->OnUnitDie.RemoveAll(this);
	}
	CurrentInputPackage.SelectedActors.Empty();
	OnSelectedChanged.Broadcast(CurrentInputPackage.SelectedActors);
}

void AOperator::ComponentAddToActionList(UUnitActionComponent* Target)
{
	if(!IsValid(Target)) return;
	for (const FName& CurrentActionName : Target->ActionList)
	{
		UActionTargetContainer** Finder = AvailableActions.Find(CurrentActionName);
		UActionTargetContainer* CurrentContainer = Finder ? *Finder : nullptr;
		if (!CurrentContainer)
		{
			CurrentContainer = AvailableActions.Add(CurrentActionName, NewObject<UActionTargetContainer>(this));
			CurrentContainer->Action = UActionSetting::GetAction(CurrentActionName);
		}
		if(CurrentContainer) CurrentContainer->Components.AddUnique(Target);
	}
}

void AOperator::ComponentRemoveFromActionList(UUnitActionComponent* Target)
{
	if(!IsValid(Target)) return;
	for (const FName& CurrentActionName : Target->ActionList)
	{
		UActionTargetContainer** Finder = AvailableActions.Find(CurrentActionName);
		UActionTargetContainer* CurrentContainer = Finder ? *Finder : nullptr;
		if (CurrentContainer)
		{
			CurrentContainer->Components.Remove(Target);
			if (CurrentContainer->Components.Num() == 0)
			{
				CurrentContainer->ConditionalBeginDestroy();
				AvailableActions.Remove(CurrentActionName);
			}
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

void AOperator::SimpleAction(const FInputPackage& Input)
{
	TMap<AActionBase*, TSet<UUnitActionComponent*>> ExecuteActionComponentMap;
	TMap<AActionBase*, TSet<AActor*>> ExecuteActionActorMap;

	for (AActor* CurrentActor : Input.SelectedActors)
	{
		AUnitBase* CurrentAsUnit = Cast<AUnitBase>(CurrentActor);
		if (!IsValid(CurrentAsUnit)) continue;
		AActionBase* ResultAction = nullptr;
		TArray<UUnitActionComponent*> ResultComponents;
		if (!CurrentAsUnit->GetSimpleAction(Input, ResultAction, ResultComponents)) continue;
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

	if (bIsReservationMode)
	{
		for (auto& CurrentPair : ExecuteActionActorMap)
		{
			AActionBase* CurrentAction = CurrentPair.Key;
			TSet<AActor*>& CurrentList = CurrentPair.Value;
			if (!IsValid(CurrentAction) || CurrentList.Num() == 0) continue;
			const TArray<AActor*> ResultArray = CurrentList.Array();

			ReservationAction(CurrentAction, ResultArray, CurrentInputPackage);
		}
	}
	else
	{
		for (auto& CurrentPair : ExecuteActionComponentMap)
		{
			AActionBase* CurrentAction = CurrentPair.Key;
			TSet<UUnitActionComponent*>& CurrentList = CurrentPair.Value;
			if (!IsValid(CurrentAction) || CurrentList.Num() == 0) continue;
			const TArray<UUnitActionComponent*> ResultArray = CurrentList.Array();
			CommandAction(CurrentAction, ResultArray, true);
		}
	}
}

void AOperator::ReservationAction(AActionBase* TargetAction, TArray<AActor*> TargetActors, const FInputPackage& Input)
{
	FActionReservator Reservator(this, TargetAction, Input);
	for (AActor* CurrentActor : TargetActors)
	{
		if (AUnitBase* AsUnit = Cast<AUnitBase>(CurrentActor))
		{
			AsUnit->ReservationEnqueue(Reservator);
		};
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

