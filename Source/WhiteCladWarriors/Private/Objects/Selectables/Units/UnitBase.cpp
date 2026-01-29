// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Selectables/Units/UnitBase.h"
#include "Generals/ReservedActionMessage.h"
#include "Actions/UnitActionComponent.h"
#include "Actions/ActionExecutor.h"
#include "Actions/ActionBase.h"
#include "Settings/ActionSetting.h"

bool FMontageEventInfo::ValidExecutor() const 
{
	return IsValid(MontageExecutor) && IsValid(MontageComponent);
}


void FMontageEventInfo::MontageNotifyBegin(FName NotifyName)
{
	if(!ValidExecutor() || !bIsStarted) return;
	OnMontageNotifyBegin.Execute(MontageExecutor.Get(), MontageComponent.Get(), RequestedID, NotifyName);
}

void FMontageEventInfo::MontageNotifyEnd(FName NotifyName)
{
	if(!ValidExecutor() || !bIsStarted) return;
	OnMontageNotifyEnd.Execute(MontageExecutor.Get(), MontageComponent.Get(), RequestedID, NotifyName);
}

void FMontageEventInfo::MontageStart()
{
	if(!ValidExecutor() || bIsStarted) return;
	OnMontageStart.Execute(MontageExecutor.Get(), MontageComponent.Get(), RequestedID);
	bIsStarted = true;
}

void FMontageEventInfo::MontageEnd(bool bIsInterrupted)
{
	if(!ValidExecutor() || !bIsStarted) return;
	OnMontageEnd.Execute(MontageExecutor.Get(), MontageComponent.Get(), RequestedID, bIsInterrupted);
	Clear();
}


void FMainActionInfo::Clear()
{
	Executor = nullptr;
	Component = nullptr;
	bIsCancelable = true;
	bIsStopMovement = false;
}

void FMainActionInfo::Set(UActionExecutor* WantExecutor, UUnitActionComponent* WantComponent, bool bWantIsCancelable, bool bWantIsStopMovement)
{
	Executor = WantExecutor;
	Component = WantComponent;
	bIsCancelable = bWantIsCancelable;
	bIsStopMovement = bWantIsStopMovement;
}

void FMainActionInfo::Clear(const UActionExecutor* OldExecutor) 
{ 
	if (!Executor.IsValid() || Executor.Get() == OldExecutor) Clear();
}

void FMainActionInfo::SetActionMessage_Simple(FName Message)
{
	if (IsValid()) Executor.Get()->SetActionMessage_Simple(Component.Get(), 0, Message);
}

bool FMainActionInfo::Cancel(bool bWantStopMovement)
{
	if(!IsValid()) return true;
	if (!bIsCancelable) return false;

	if (UUnitActionComponent* TargetComponent = Component.Get())
	{
		Executor->CancelMainNode(TargetComponent);
		TargetComponent->OnEndMainAction(Executor.Get(), bWantStopMovement);
	}
	Clear();
	return true;
}

void FMainActionInfo::End(bool bWantStopMovement)
{
	if (IsValid())
	{
		Component.Get()->OnEndMainAction(Executor.Get(), bWantStopMovement);
		Clear();
	}
}


bool FMainActionInfo::IsValid() const
{
	return Executor.IsValid() && Component.IsValid();
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
	if(Info.IsValid()) return SetMainAction(Info.Executor.Get(), Info.Component.Get(), Info.bIsCancelable);
	else return SetMainAction(nullptr, nullptr);
}

bool AUnitBase::GetMainActionCancelable_Implementation() const 
{ return MainAction.bIsCancelable; };

bool AUnitBase::SetMainAction(UActionExecutor* Executor, UUnitActionComponent* Component, bool bIsCancelable, bool bIsStopMovement)
{
	bool Result = MainAction.Cancel(bIsStopMovement);
	if (Result)
	{
		if(bIsStopMovement) ClaimStopMovement();
		MainAction.Set(Executor, Component, bIsCancelable);
	}
	return Result;
}

void AUnitBase::EndMainAction(bool bIsStopMovement)
{
	if (MainAction.IsValid())
	{
		if (bIsStopMovement) ClaimStopMovement();
		MainAction.End(bIsStopMovement);
	}
}

void AUnitBase::EndMainAction(UActionExecutor* OldExecutor, bool bIsStopMovement)
{
	EndMainAction(bIsStopMovement);
}

void AUnitBase::ReservationEnqueue(const FActionReservator& Reservation)
{
	ActionQueue.Enqueue(Reservation);
};

void AUnitBase::ReservationClear()
{
	ActionQueue.Empty();
};

void AUnitBase::ReservationNext()
{

}

void AUnitBase::NotifyExecutorEnded_Implementation(UActionExecutor* EndExecutor, UUnitActionComponent* EndComponent)
{
	if (FActionReservator* CurrentReservator = ActionQueue.Peek())
	{
		if (CurrentReservator->SetEnd(EndExecutor, EndComponent))
		{
			ReservationNext();
		}
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

void AUnitBase::NotifyMontageNodePassed_Implementation(UActionExecutor* MontageExecutor, int RequestedID)
{
	if (ClaimedMontageEvent.MontageExecutor == MontageExecutor && ClaimedMontageEvent.RequestedID == RequestedID)
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

void AUnitBase::ClaimStartMovement_Implementation(const FVector& Destination, AActor* TargetActor, float AcceptanceRadius, UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID)
{
	OnMovementStart.Broadcast(Destination, TargetActor, AcceptanceRadius, Executor, TargetComponent, ID);
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
