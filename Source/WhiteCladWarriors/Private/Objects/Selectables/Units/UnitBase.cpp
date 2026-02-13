// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Selectables/Units/UnitBase.h"
#include "Objects/Selectables/Components/UnitActionComponent.h"
#include "Generals/ReservedActionMessage.h"
#include "Actions/ActionExecutor.h"
#include "Actions/ActionBase.h"
#include "Settings/ActionSetting.h"

void FActionReservator::Clear()
{
	bIsValid = false;
	Cursor.Clear();
	RunningComponents.Empty();
}

bool FActionReservator::CheckValid()
{
	bool CursorValid = Cursor.CheckValid();
	if (!bIsValid)
	{
		if (CursorValid)
		{
			Clear();
			CursorValid = false;
		}
	}
	else if (CursorValid)
	{
		Clear();
	}
	return CursorValid;
}

bool FActionReservator::Run(TArray<UUnitActionComponent*> StartComponents)
{
	if (!IsValid(Cursor.CurrentAction)) return bIsValid = false;
	RunningComponents = StartComponents;
	Cursor.CurrentExecutor = Cursor.CurrentAction->ExecuteActionWithInput(Cursor.CurrentOperator, RunningComponents, Input);
	bIsValid = IsValid(Cursor.CurrentExecutor);
	return bIsValid;
}

bool FActionReservator::SetEnd(UActionExecutor* EndExecutor, UUnitActionComponent* EndComponent)
{
	if (!CheckValid()) return false;
	if (!IsValid(EndExecutor) || Cursor.CurrentExecutor != EndExecutor) return false;
	
	RunningComponents.Remove(EndComponent);
	bIsValid = !RunningComponents.IsEmpty();
	return !bIsValid;
}

bool FMontageEventInfo::ValidExecutor() const 
{
	return IsValid(Cursor.CurrentExecutor) && IsValid(Cursor.CurrentComponent) && IsValid(MontageToPlay);
}


void FMontageEventInfo::MontageNotifyBegin(FName NotifyName)
{
	if(!ValidExecutor() || !bIsStarted) return;
	OnMontageNotifyBegin.ExecuteIfBound(Cursor, NotifyName);
}

void FMontageEventInfo::MontageNotifyEnd(FName NotifyName)
{
	if(!ValidExecutor() || !bIsStarted) return;
	OnMontageNotifyEnd.ExecuteIfBound(Cursor, NotifyName);
}

void FMontageEventInfo::MontageStart()
{
	if (!ValidExecutor() || bIsStarted) return;
	bIsStarted = true;
	OnMontageStart.ExecuteIfBound(Cursor);
}

void FMontageEventInfo::MontageEnd(bool bIsInterrupted)
{
	if (!ValidExecutor() || !bIsStarted) return;
	OnMontageEnd.ExecuteIfBound(Cursor, bIsInterrupted);
	Clear();
}


void FMainActionInfo::Clear()
{
	Cursor.Clear();
	bIsCancelable = true;
	bIsStopMovement = false;
}

void FMainActionInfo::Set(const FActionCursorFinder& WantCursor, bool bWantIsCancelable, bool bWantIsStopMovement)
{
	Cursor = WantCursor;
	bIsCancelable = bWantIsCancelable;
	bIsStopMovement = bWantIsStopMovement;
}

void FMainActionInfo::Clear(const UActionExecutor* OldExecutor) 
{ 
	if (Cursor.CheckExecutor(OldExecutor)) Clear();
}

void FMainActionInfo::SetActionMessage_Simple(FName Message)
{
	if (CheckValid()) Cursor.CurrentExecutor->SetActionMessage_Simple(Cursor, Message);
}

bool FMainActionInfo::Cancel(bool bWantStopMovement)
{
	if(!CheckValid()) return true;
	if (!bIsCancelable) return false;

	if (UUnitActionComponent* TargetComponent = Cursor.CurrentComponent)
	{
		Cursor.CurrentExecutor->CancelNode(Cursor);
		//TargetComponent->OnEndMainAction(Executor, bWantStopMovement);
	}
	Clear();
	return true;
}

void FMainActionInfo::End(bool bWantStopMovement)
{
	if (CheckValid())
	{
		Cursor.CurrentComponent->OnEndMainAction(Cursor.CurrentExecutor, bWantStopMovement);
		Clear();
	}
}


bool FMainActionInfo::CheckValid() const
{
	return Cursor.CheckValid();
}

void AUnitBase::BeginPlay()
{
	Super::BeginPlay();
	
	for (UActorComponent* CurrentComponent : GetComponents()) AddActionComponent(Cast<UUnitActionComponent>(CurrentComponent));

	if (USkeletalMeshComponent* CurrentMesh = GetMesh())
	{
		if (UAnimInstance* AnimInstance = CurrentMesh->GetAnimInstance())
		{
			AnimInstance->OnMontageStarted.AddDynamic(this, &AUnitBase::MontageStarted);
			AnimInstance->OnMontageEnded.AddDynamic(this, &AUnitBase::MontageEnded);
			AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &AUnitBase::MontageNotifyBegin);
			AnimInstance->OnPlayMontageNotifyEnd.AddDynamic(this, &AUnitBase::MontageNotifyEnd);
		}
	}
}

void AUnitBase::BeginDestroy()
{
	Super::BeginDestroy();
	//MontageComponent.Reset();
	//MontageExecutor.Reset();
}

TArray<AActionBase*> AUnitBase::GetActionList() const
{
	TArray<AActionBase*> Result;
	ActionMap.GetKeys(Result);
	return Result;
}

TArray<UUnitActionComponent*> AUnitBase::GetComponentsWithAction(AActionBase* TargetAction) const
{
	if (const TArray<UUnitActionComponent*>* Result = ActionMap.Find(TargetAction)) return *Result;
	else return TArray<UUnitActionComponent*>();
}

bool AUnitBase::GetSimpleAction(const FInputPackage& CurrentInput, AActionBase*& OutAction, TArray<UUnitActionComponent*>& OutComponents) const
{
	int MaxOrder = 0;
	AActionBase* MaxAction = nullptr;
	for (const TPair<AActionBase*, TArray<UUnitActionComponent*>>& CurrentPair : ActionMap)
	{
		AActionBase* CurrentAction = CurrentPair.Key;
		if (!IsValid(CurrentAction)) continue;
		for (UUnitActionComponent* CurrentComponent : CurrentPair.Value)
		{
			if (!IsValid(CurrentComponent)) continue;

			int CurrentOrder = CurrentAction->GetSimpleActionOrder(CurrentInput, CurrentComponent);

			if (CurrentOrder > MaxOrder)
			{
				MaxOrder = CurrentOrder;
				MaxAction = CurrentAction;
			}
		}
	}
	if (MaxAction == nullptr) return false;

	OutAction = MaxAction;
	const TArray<UUnitActionComponent*>* ResultComponents = ActionMap.Find(OutAction);
	OutComponents = *ResultComponents;
	return MaxOrder > 0;
}

void AUnitBase::AddActionComponent(UUnitActionComponent* NewComponent)
{
	if (!IsValid(NewComponent)) return;

	ActionComponentArray.AddUnique(NewComponent);

	for (FName CurrentName : NewComponent->ActionList)
	{
		if (AActionBase* CurrentAction = UActionSetting::GetAction(CurrentName))
		{
			TArray<UUnitActionComponent*>& CurrentList = ActionMap.FindOrAdd(CurrentAction);
			CurrentList.Add(NewComponent);
		}
	}
};


bool AUnitBase::SetMainAction(const FMainActionInfo& Info)
{
	if(Info.CheckValid()) return SetMainAction(Info.Cursor, Info.bIsCancelable);
	else return SetMainAction(FActionCursorFinder::None);
}

bool AUnitBase::GetMainActionCancelable_Implementation() const 
{ return MainAction.bIsCancelable; };

bool AUnitBase::SetMainAction(const FActionCursorFinder& WantCursor, bool bIsCancelable, bool bIsStopMovement)
{
	bool Result = MainAction.Cancel(bIsStopMovement);
	if (Result)
	{
		if(bIsStopMovement) ClaimStopMovement();
		MainAction.Set(WantCursor, bIsCancelable, bIsStopMovement);
	}
	return Result;
}

void AUnitBase::EndMainAction(UActionExecutor* OldExecutor, UUnitActionComponent* OldComponent, bool bIsStopMovement)
{
	if (!MainAction.CheckValid() || MainAction.Cursor.CurrentExecutor != OldExecutor|| MainAction.Cursor.CurrentComponent != OldComponent) return;

	if (bIsStopMovement) ClaimStopMovement();
	MainAction.End(bIsStopMovement);

	if (CurrentReservatedAction.bIsValid)
	{
		if (CurrentReservatedAction.SetEnd(OldExecutor, OldComponent)) ReservationNext();
	}
	else
	{
		ReservationNext();
	}
}

void AUnitBase::ReservationEnqueue(const FActionReservator& Reservation)
{
	const FActionReservator& NewReservation = Reservation;

	if (MainAction.CheckValid() || CurrentReservatedAction.CheckValid())
	{
		ActionQueue.Enqueue(NewReservation);
	}
	else
	{
		CurrentReservatedAction = NewReservation;
		if (!CurrentReservatedAction.Run(GetComponentsWithAction(CurrentReservatedAction.Cursor.CurrentAction)))
		{
			ReservationNext();
		}
	}
};

void AUnitBase::ReservationClear()
{
	ActionQueue.Empty();
};

void AUnitBase::ReservationNext()
{
	if (ActionQueue.Dequeue(CurrentReservatedAction))
	{
		if (!CurrentReservatedAction.Run(GetComponentsWithAction(CurrentReservatedAction.Cursor.CurrentAction)))
		{
			ReservationNext();
		}
	}
	else if(CurrentReservatedAction.CheckValid())
	{
		CurrentReservatedAction.Clear();
	}
}

void AUnitBase::NotifyExecutorEnded_Implementation(UActionExecutor* EndExecutor, UUnitActionComponent* EndComponent)
{
	if (CurrentReservatedAction.CheckValid())
	{
		if (CurrentReservatedAction.SetEnd(EndExecutor, EndComponent))
		{
			ReservationNext();
		}
	}
	else
	{
		ReservationNext();
	}
}


void AUnitBase::ClaimPlayMontage_Implementation(const FMontageEventInfo& MontageEvent)
{
	if (USkeletalMeshComponent* CurrentMesh = GetMesh())
	{
		if (UAnimInstance* AnimInstance = CurrentMesh->GetAnimInstance())
		{
			if (ClaimedMontageEvent.ValidExecutor())
			{
				QueuedMontageEvent = MontageEvent;
				AnimInstance->Montage_Stop(0.0f, ClaimedMontageEvent.MontageToPlay);
			}
			else
			{
				ClaimedMontageEvent = MontageEvent;
				AnimInstance->Montage_Play(ClaimedMontageEvent.MontageToPlay, ClaimedMontageEvent.PlayRate, EMontagePlayReturnType::MontageLength, ClaimedMontageEvent.StartingPosition);
			}
		}
	}
}

void AUnitBase::NotifyMontageNodePassed_Implementation(const FActionCursorFinder& WantCursor)
{
	if (WantCursor.CheckExecutor(ClaimedMontageEvent.Cursor.CurrentExecutor) && ClaimedMontageEvent.Cursor.CurrentID == WantCursor.CurrentID)
	{
		ClaimedMontageEvent.MontageToPlay = nullptr;
	}
}


void AUnitBase::ClaimStopMontage_Implementation(UAnimMontage* WantMontage)
{
	if (USkeletalMeshComponent* CurrentMesh = GetMesh())
	{
		if (UAnimInstance* AnimInstance = CurrentMesh->GetAnimInstance())
		{
			AnimInstance->Montage_Stop(0.2f, WantMontage);
		}
	}
}

void AUnitBase::ClaimStartMovement_Implementation(const FVector& Destination, AActor* TargetActor, float AcceptanceRadius, const FActionCursorFinder& WantCursor)
{
	OnMovementStart.Broadcast(Destination, TargetActor, AcceptanceRadius, WantCursor);
}

void AUnitBase::ClaimStopMovement_Implementation()
{
	OnMovementStop.Broadcast();
}

bool AUnitBase::ClaimJump_Implementation()
{
	Jump();
	return true;
}

void AUnitBase::MontageStarted(UAnimMontage* Montage)
{
	ClaimedMontageEvent.MontageStart();
}

void AUnitBase::MontageEnded(UAnimMontage* Montage, bool bIsInterrupted)
{
	if(ClaimedMontageEvent.MontageToPlay == Montage)
	{
		ClaimedMontageEvent.MontageEnd(bIsInterrupted);
		if (QueuedMontageEvent.ValidExecutor())
		{
			ClaimedMontageEvent = QueuedMontageEvent;
			QueuedMontageEvent.Clear();
			if (USkeletalMeshComponent* CurrentMesh = GetMesh())
			{
				if (UAnimInstance* AnimInstance = CurrentMesh->GetAnimInstance())
				{
					AnimInstance->Montage_Play(ClaimedMontageEvent.MontageToPlay, ClaimedMontageEvent.PlayRate, EMontagePlayReturnType::MontageLength, ClaimedMontageEvent.StartingPosition);
				}
			}
		}
	}
}

void AUnitBase::MontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	ClaimedMontageEvent.MontageNotifyBegin(NotifyName);
}

void AUnitBase::MontageNotifyEnd(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	ClaimedMontageEvent.MontageNotifyEnd(NotifyName);
}


void AUnitBase::Die_Implementation() 
{ 
	for (UActorComponent* CurrentComponent : GetComponents())
	{
		if (UUnitComponentBase* AsUnitComponent = Cast<UUnitComponentBase>(CurrentComponent)) AsUnitComponent->BroadcastRemoveMessage();
	}
	OnUnitDie.Broadcast(this); 
}
