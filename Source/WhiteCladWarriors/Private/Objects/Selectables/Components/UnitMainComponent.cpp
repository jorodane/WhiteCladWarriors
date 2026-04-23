// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Selectables/Components/UnitMainComponent.h"
#include "Objects/Selectables/Components/UnitComponentBase.h"
#include "Objects/Selectables/Components/UnitActionComponent.h"
#include "Objects/Selectables/Components/FillableValueComponent.h"
#include "Objects/Players/IngameController.h"
#include "Objects/Players/Operator.h"
#include "Generals/ReservedActionMessage.h"
#include "Actions/Executables/ActionExecutor.h"
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

void UUnitMainComponent::BeginPlay()
{
	for (UActorComponent* CurrentComponent : GetOwner()->GetComponents())
	{
		if (CurrentComponent == this) continue;
		AddUnitComponent(Cast<UUnitComponentBase>(CurrentComponent));
	}
	SetMesh(GetMesh());

	Super::BeginPlay();
}

TArray<UUnitComponentBase*> UUnitMainComponent::GetComponents() const
{
	return UnitComponentArray;
}

USkeletalMeshComponent* UUnitMainComponent::GetMesh_Implementation() const
{
	if (ACharacter* OwnerAsCharacter = Cast<ACharacter>(GetOwner()))
	{
		return OwnerAsCharacter->GetMesh();
	};
	return nullptr;
}

USkeletalMeshComponent* UUnitMainComponent::SetMesh_Implementation(USkeletalMeshComponent* NewMesh)
{
	if (USkeletalMeshComponent* CurrentMesh = NewMesh)
	{
		AnimInstance = CurrentMesh->GetAnimInstance();
		if(IsValid(AnimInstance))
		{
			AnimInstance->OnMontageStarted.AddDynamic(this, &UUnitMainComponent::MontageStarted);
			AnimInstance->OnMontageEnded.AddDynamic(this, &UUnitMainComponent::MontageEnded);
			AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UUnitMainComponent::MontageNotifyBegin);
			AnimInstance->OnPlayMontageNotifyEnd.AddDynamic(this, &UUnitMainComponent::MontageNotifyEnd);
		}
		return CurrentMesh;
	}
	return nullptr;
}

bool UUnitMainComponent::AddFillValue(FName WantTag, UFillableValueComponent* Target)
{
	if (!FillValueMap.Contains(WantTag))
	{
		FillValueMap.Add(WantTag, Target);
		OnFillValueAdded.Broadcast(Target);
		return true;
	}
	return false;
}

void UUnitMainComponent::RemoveFillValue(FName WantTag)
{
	UFillableValueComponent** Finder = FillValueMap.Find(WantTag);
	if (Finder)
	{
		if (UFillableValueComponent* Result = *Finder)
		{
			OnFillValueRemoved.Broadcast(Result);
		}
		FillValueMap.Remove(WantTag);
	}
}

UFillableValueComponent* UUnitMainComponent::FindFillValue(FName WantTag)
{
	UFillableValueComponent** Finder = FillValueMap.Find(WantTag);
	if (Finder) return *Finder;
	else return nullptr;
}

bool UUnitMainComponent::TryFindFillValue(FName WantTag, UFillableValueComponent*& Result)
{
	Result = FindFillValue(WantTag);
	return Result != nullptr;
}

TArray<UFillableValueComponent*> UUnitMainComponent::FindAllFillValue()
{
	TArray<UFillableValueComponent*> Result;
	FillValueMap.GenerateValueArray(Result);
	return Result;
}

bool UUnitMainComponent::HasOperatorAuthority_Implementation(AOperator* From)
{
	if (PlayerController == nullptr || From == nullptr) return false;
	return PlayerController->ConnectedOperator == From;
}

bool UUnitMainComponent::IsAlly_Implementation(EUnitControlledType OtherType)
{
	if (OtherType == ControlledType) return true;

	if (ControlledType == EUnitControlledType::Monster || OtherType == EUnitControlledType::Monster)
	{
		return false;
	}
	else return true;
}

EUnitAllyType UUnitMainComponent::GetAllyType_Implementation(AOperator* From)
{
	if (PlayerController == nullptr)
	{
		if (ControlledType == EUnitControlledType::Monster)
		{
			return EUnitAllyType::Enemy;
		}
		else
		{
			return EUnitAllyType::Normal;
		}
	}

	if (HasOperatorAuthority(From))
	{
		return EUnitAllyType::Own;
	}
	else
	{
		return EUnitAllyType::Ally;
	}
}


TArray<AActionBase*> UUnitMainComponent::GetActionList() const
{
	TArray<AActionBase*> Result;
	ActionMap.GetKeys(Result);
	return Result;
}

TArray<AActionBase*> UUnitMainComponent::GetActionFromKey(FKey WantKey) const
{
	TArray<AActionBase*> Result;
	for (const TPair<AActionBase*, FActionTargetContainer>& CurrentPair : ActionMap)
	{
		if (AActionBase* CurrentAction = CurrentPair.Key)
		{
			if (CurrentAction->GetHotKey() == WantKey) Result.Add(CurrentAction);
		}
	}
	return Result;
}

TArray<FActionTargetContainer> UUnitMainComponent::GetActionContainerFromKey(FKey WantKey) const
{
	TArray<FActionTargetContainer> Result;
	for (const TPair<AActionBase*, FActionTargetContainer>& CurrentPair : ActionMap)
	{
		if (AActionBase* CurrentAction = CurrentPair.Key)
		{
			if (CurrentAction->GetHotKey() == WantKey) Result.Add(CurrentPair.Value);
		}
	}
	return Result;
}

TArray<UUnitActionComponent*> UUnitMainComponent::GetComponentsWithAction(AActionBase* TargetAction) const
{
	if (const FActionTargetContainer* Result = ActionMap.Find(TargetAction)) return Result->Components;
	else return TArray<UUnitActionComponent*>();
}

bool UUnitMainComponent::GetSimpleAction(const FInputPackage& CurrentInput, AActionBase*& OutAction, TArray<UUnitActionComponent*>& OutComponents) const
{
	int MaxOrder = 0;
	AActionBase* MaxAction = nullptr;
	for (const TPair<AActionBase*, FActionTargetContainer>& CurrentPair : ActionMap)
	{
		AActionBase* CurrentAction = CurrentPair.Key;
		if (!IsValid(CurrentAction)) continue;
		for (UUnitActionComponent* CurrentComponent : CurrentPair.Value.Components)
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
	if (ActionMap.Find(OutAction))
	OutComponents = GetComponentsWithAction(OutAction);
	return MaxOrder > 0;
}

void UUnitMainComponent::AddUnitComponent(UUnitComponentBase* NewComponent)
{
	if (!IsValid(NewComponent)) return;

	NewComponent->SetOwnerUnit(this);
	UnitComponentArray.AddUnique(NewComponent);

	if (UUnitActionComponent* AsActionComponent = Cast<UUnitActionComponent>(NewComponent))
	{
		for (FName CurrentName : AsActionComponent->ActionList)
		{
			if (AActionBase* CurrentAction = UActionSetting::GetAction(CurrentName))
			{
				FActionTargetContainer* CurrentContainer = ActionMap.Find(CurrentAction);
				if (CurrentContainer == nullptr) CurrentContainer = &ActionMap.Add(CurrentAction);
				if (CurrentContainer != nullptr) CurrentContainer->Components.Add(AsActionComponent);
			}
		}
	}
};


bool UUnitMainComponent::SetMainAction(const FMainActionInfo& Info)
{
	if(Info.CheckValid()) return SetMainAction(Info.Cursor, Info.bIsCancelable);
	else return SetMainAction(FActionCursorFinder::None);
}

bool UUnitMainComponent::GetMainActionCancelable_Implementation() const 
{ return MainAction.bIsCancelable; };

bool UUnitMainComponent::SetMainAction(const FActionCursorFinder& WantCursor, bool bIsCancelable, bool bIsStopMovement)
{
	bool Result = MainAction.Cancel(bIsStopMovement);
	if (Result)
	{
		if(bIsStopMovement) ClaimStopMovement();
		MainAction.Set(WantCursor, bIsCancelable, bIsStopMovement);
	}
	return Result;
}

void UUnitMainComponent::EndMainAction(UActionExecutor* OldExecutor, UUnitActionComponent* OldComponent, bool bIsStopMovement)
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

void UUnitMainComponent::ReservationEnqueue(const FActionReservator& Reservation)
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

void UUnitMainComponent::ReservationClear()
{
	ActionQueue.Empty();
};

void UUnitMainComponent::ReservationNext()
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

void UUnitMainComponent::NotifyExecutorEnded_Implementation(UActionExecutor* EndExecutor, UUnitActionComponent* EndComponent)
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

void UUnitMainComponent::NotifyMontageNodePassed_Implementation(const FActionCursorFinder& WantCursor)
{
	ClaimedMontageEvent.MontageToPlay = nullptr;
}

bool UUnitMainComponent::PlayInputReadyMontage_Implementation(const FMontageEventInfo& MontageEvent)
{
	if (!IsValid(AnimInstance)) return false;
	InputReadyMontageEvent.Stop(AnimInstance);
	InputReadyMontageEvent = MontageEvent;
	InputReadyMontageEvent.Play(AnimInstance);
	return true;
}

void UUnitMainComponent::StopInputReadyMontage_Implementation()
{
	if (!IsValid(AnimInstance)) return;
	InputReadyMontageEvent.Stop(AnimInstance);
	InputReadyMontageEvent.Clear();
}

bool UUnitMainComponent::ClaimPlayMontage_Implementation(const FMontageEventInfo& MontageEvent)
{
	if (IsValid(AnimInstance))
	{

	}
	return true;
}

bool UUnitMainComponent::ClaimStopMontage_Implementation(UAnimMontage* WantMontage)
{
	if (IsValid(AnimInstance))
	{
		AnimInstance->Montage_Stop(0.2f, WantMontage);
	}
	return true;
}

bool UUnitMainComponent::ClaimStartMovement_Implementation(const FVector& Destination, AActor* TargetActor, float AcceptanceRadius, const FActionCursorFinder& WantCursor)
{
	OnMovementStart.Broadcast(Destination, TargetActor, AcceptanceRadius, WantCursor);
	return true;
}

bool UUnitMainComponent::ClaimStopMovement_Implementation()
{
	OnMovementStop.Broadcast();
	return true;
}

void UUnitMainComponent::UnitMessage(const FName& Message)
{
	OnUnitMessage_Simple.Broadcast(Message);
}

void UUnitMainComponent::MontageStarted(UAnimMontage* Montage)
{
}

void UUnitMainComponent::MontageEnded(UAnimMontage* Montage, bool bIsInterrupted)
{
}

void UUnitMainComponent::MontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
}

void UUnitMainComponent::MontageNotifyEnd(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
}


void UUnitMainComponent::Die_Implementation() 
{ 
	for (UActorComponent* CurrentComponent : GetComponents())
	{
		if (UUnitComponentBase* AsUnitComponent = Cast<UUnitComponentBase>(CurrentComponent)) AsUnitComponent->BroadcastMessage_Removed();
	}
	OnUnitDie.Broadcast(this); 
}

TArray<UOrderedGenericWidgetClaim*> UUnitMainComponent::GetInfoWidget_Implementation(EInfoWidgetType WantType, AOperator* Operator) const
{
	TArray<UOrderedGenericWidgetClaim*> Result;
	Result.Append(GetUnitInfoWidget(WantType));
	for (UActorComponent* CurrentComponent : GetComponents())
	{
		if (CurrentComponent && CurrentComponent->GetClass()->ImplementsInterface(UInfoConnectable::StaticClass()))
		{
			Result.Append(IInfoConnectable::Execute_GetInfoWidget(CurrentComponent, WantType, Operator));
		}
	};
	return Result;
}

void UUnitMainComponent::OnPlayerConnected_Implementation(AIngameController* NewPlayer)
{
	if (PlayerController) IPlayerConnectable::Execute_OnPlayerDisconnected(this, PlayerController);

	PlayerController = NewPlayer;
}

void UUnitMainComponent::OnPlayerDisconnected_Implementation(AIngameController* OldPlayer)
{
	PlayerController = nullptr;
}
