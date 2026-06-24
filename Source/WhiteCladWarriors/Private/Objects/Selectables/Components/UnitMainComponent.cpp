// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Selectables/Components/UnitMainComponent.h"
#include "Objects/Selectables/Components/UnitComponentBase.h"
#include "Objects/Selectables/Components/UnitActionComponent.h"
#include "Objects/Values/FillableValue.h"
#include "Objects/Players/IngameController.h"
#include "Objects/Players/Operator.h"
#include "Generals/ReservedActionMessage.h"
#include "Actions/Executables/ActionExecutor.h"
#include "Actions/Executables/ActionNode.h"
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
	UActionExecutor* CreatedExecutor = Cursor.CurrentAction->ExecuteActionWithInput(Cursor.CurrentOperator, RunningComponents, FExecutorValueMap(), Input);
	bIsValid = IsValid(CreatedExecutor);
	return bIsValid;
}

bool FActionReservator::SetEnd(int64 EndExecutorID, UUnitActionComponent* EndComponent)
{
	if (!CheckValid()) return false;
	UActionExecutor* EndExecutor = UActionExecutor::GetExecutorFromID(EndExecutorID);
	if (Cursor.CurrentExecutorID != EndExecutorID || !IsValid(EndExecutor)) return false;
	
	RunningComponents.Remove(EndComponent);
	bIsValid = !RunningComponents.IsEmpty();
	return !bIsValid;
}

UUnitMainComponent::UUnitMainComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UUnitMainComponent::BeginPlay()
{
	OwnerUnit = this;
	for (UActorComponent* CurrentComponent : GetOwner()->GetComponents())
	{
		if (CurrentComponent == this) continue;
		AddUnitComponent(Cast<UUnitComponentBase>(CurrentComponent));
	}
	if (ACharacter* OwnerAsCharacter = Cast<ACharacter>(GetOwner()))
	{
		SetMesh(OwnerAsCharacter->GetMesh());
	};

	Super::BeginPlay();
}

void UUnitMainComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	USkeletalMeshComponent* CurrentMesh = GetMesh();
	if (IsValid(CurrentMesh))
	{
		FRotator ResultRotation(0.0, -90.0, 0.0);
		FVector FocusLocation;
		if (!CurrentMesh->IsAnySimulatingPhysics())
		{
			if (GetFocusLocation(FocusLocation))
			{
				FVector FocusDirection = (FocusLocation - CurrentMesh->GetComponentLocation()).GetSafeNormal2D();
				double FocusRadian = FMath::Atan2(FocusDirection.Y, FocusDirection.X);
				ResultRotation.Yaw += FMath::RadiansToDegrees(FocusRadian);
				CurrentMesh->SetWorldRotation(ResultRotation);
			}
			else if(!MainActionMontageEvent.bIsStarted)
			{
				FRotator LastRotator = CurrentMesh->GetRelativeRotation();
				CurrentMesh->SetRelativeRotation(FMath::Lerp(ResultRotation, LastRotator, 0.95));
			}
			else
			{
				CurrentMesh->SetRelativeRotation(ResultRotation);
			}
		}
	}
}

TArray<UUnitComponentBase*> UUnitMainComponent::GetComponents() const
{
	return UnitComponentArray;
}

USkeletalMeshComponent* UUnitMainComponent::GetMesh_Implementation() const
{
	return Mesh;
}

USkeletalMeshComponent* UUnitMainComponent::SetMesh_Implementation(USkeletalMeshComponent* NewMesh)
{
	if (USkeletalMeshComponent* CurrentMesh = NewMesh)
	{
		AnimInstance = CurrentMesh->GetAnimInstance();
		if(IsValid(AnimInstance))
		{
			AnimInstance->OnMontageStarted.AddDynamic(this, &UUnitMainComponent::MontageStarted);
			//AnimInstance->OnMontageEnded.AddDynamic(this, &UUnitMainComponent::MontageEnded);
			AnimInstance->OnMontageBlendingOut.AddDynamic(this, &UUnitMainComponent::MontageEnded);
			AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UUnitMainComponent::MontageNotifyBegin);
			AnimInstance->OnPlayMontageNotifyEnd.AddDynamic(this, &UUnitMainComponent::MontageNotifyEnd);
		}
	}
	Mesh = NewMesh;
	return Mesh;
}


bool UUnitMainComponent::HasOperatorAuthority_Implementation(AOperator* From) const
{
	if (From == nullptr) return false;
	return GetOperator() == From;
}

AOperator* UUnitMainComponent::GetOperator() const
{
	if (!IsValid(PlayerController)) return nullptr;
	return PlayerController->ConnectedOperator;
}

FMainActionInfo UUnitMainComponent::GetMainActionInfo() const
{
	return MainAction;
}

bool UUnitMainComponent::HasMainAction() const
{
	return MainAction.CheckValid();
}

bool UUnitMainComponent::HasInputReadyMontage() const
{
	return InputReadyMontageEvent.bIsStarted;
}

FMontageEventInfo UUnitMainComponent::GetActionMontageInfo() const
{
	return MainActionMontageEvent;
}

bool UUnitMainComponent::HasActionMontage() const
{
	return MainActionMontageEvent.bIsStarted;
}


bool UUnitMainComponent::IsAlly_Implementation(EUnitControlledType OtherType) const
{
	if (OtherType == ControlledType) return true;

	if (ControlledType == EUnitControlledType::Monster || OtherType == EUnitControlledType::Monster)
	{
		return false;
	}
	else return true;
}

EUnitAllyType UUnitMainComponent::GetAllyTypeFromOperator_Implementation(AOperator* From) const
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

EUnitAllyType UUnitMainComponent::GetAllyTypeFromUnit_Implementation(UUnitMainComponent* From) const
{
	if (!IsValid(From)) return EUnitAllyType::Normal;
	else
	{
		EUnitControlledType FromController = From->ControlledType;
		if (IsValid(PlayerController) && From->PlayerController == PlayerController) return EUnitAllyType::Own;
		else if (ControlledType == EUnitControlledType::Neutral || FromController == EUnitControlledType::Neutral) return EUnitAllyType::Normal;
		else if (IsAlly(FromController)) return EUnitAllyType::Ally;
		else return EUnitAllyType::Enemy;
	}
}


bool UUnitMainComponent::GetFocusLocation_Implementation(FVector& OutResult) const
{
	if (IsValid(CurrentIntention.TargetActor))
	{
		OutResult = CurrentIntention.TargetActor->GetActorLocation();
	}
	else if (HasActionMontage() && IsValid(MainActionMontageEvent.FocusTarget))
	{
		OutResult = MainActionMontageEvent.FocusTarget->GetActorLocation();
	}
	else if (HasInputReadyMontage())
	{
		OutResult = AOperator::GetLocalInputPackage().MouseTerrainPosition;
	}
	else return false;
	
	return true;
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

FName UUnitMainComponent::GetActionFromTag(FName WantTag) const
{
	if (const FName* Result = TaggedActionMap.Find(WantTag))
	{
		return *Result;
	}
	else
	{
		return NAME_None;
	}
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

bool UUnitMainComponent::GetMainActionCancelable_Implementation() const
{
	return !MainAction.CheckValid() || MainAction.Settings.bIsCancelable;
};

bool UUnitMainComponent::GetMainActionExecutable_Implementation()
{
	return GetActionExecutable() && GetMainActionCancelable();
}

bool UUnitMainComponent::GetActionExecutable_Implementation()
{
	return !IDamageable::Execute_GetIsDie(this);
}

bool UUnitMainComponent::SetMainAction(const FMainActionInfo& Info, UActionNode* WantNode)
{
	bool Result = false;
	if(Info.CheckValid())	Result = SetMainAction(Info.Cursor, WantNode);
	else					Result = StopMainAction();
	return Result;
}

bool UUnitMainComponent::SetMainAction(const FActionCursorFinder& WantCursor, UActionNode* WantNode)
{
	if (!IsValid(WantNode)) return false;
	bool Result = GetMainActionExecutable() && MainAction.Interrupt(WantCursor, WantNode);
	if (Result)
	{
		const FActionExecuteSettingContainer& Settings = WantNode->Settings;
		if(Settings.bIsStopMovementOnStart) ClaimStopMovement();
		MainAction.Set(WantCursor, Settings);
		OnMainActionChanged.Broadcast(MainAction, true);
	}
	return Result;
}

bool UUnitMainComponent::StopMainAction()
{
	bool Result = MainAction.Cancel();
	if(Result) OnMainActionChanged.Broadcast(MainAction, false);
	return Result;
}

void UUnitMainComponent::EndMainAction(int64 OldExecutorID, UUnitActionComponent* OldComponent)
{
	UActionExecutor* OldExecutor = UActionExecutor::GetExecutorFromID(OldExecutorID);
	if (!MainAction.CheckValid() || MainAction.Cursor.CurrentExecutorID != OldExecutorID || MainAction.Cursor.CurrentComponent != OldComponent) return;

	if (MainAction.End()) OnMainActionChanged.Broadcast(MainAction, false);

	if (CurrentReservatedAction.bIsValid)
	{
		if (CurrentReservatedAction.SetEnd(OldExecutorID, OldComponent)) ReservationNext();
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

void UUnitMainComponent::NotifyExecutorEnded_Implementation(int64 EndExecutorID, UUnitActionComponent* EndComponent)
{
	if (CurrentReservatedAction.CheckValid())
	{
		if (CurrentReservatedAction.SetEnd(EndExecutorID, EndComponent))
		{
			ReservationNext();
		}
	}
	else
	{
		ReservationNext();
	}
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

bool UUnitMainComponent::PlayMainActionMontage_Implementation(const FMontageEventInfo& MontageEvent)
{
	if (!IsValid(MontageEvent.MontageToPlay)) return false;
	if (MainActionMontageEvent.bIsStarted)
	{
		if(MainActionMontageEvent.MontageToPlay != MontageEvent.MontageToPlay) StopMainActionMontage(true);
		else UnitMessage_Montage(MainActionMontageEvent.MontageToPlay, false, true);
	}
	MainActionMontageEvent = MontageEvent;
	if (MainActionMontageEvent.Play(AnimInstance, false))
	{
		UnitMessage_Montage(MainActionMontageEvent.MontageToPlay, true, false);
		return true;
	}
	return false;
}

bool UUnitMainComponent::StopMainActionMontage_Implementation(bool bIsInterrupted)
{
	if (!MainActionMontageEvent.bIsStarted) return true;
	UAnimMontage* ClearedMontage = MainActionMontageEvent.MontageToPlay;
	MainActionMontageEvent.Stop(AnimInstance);
	MainActionMontageEvent.Clear();
	UnitMessage_Montage(ClearedMontage, false, bIsInterrupted);
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

float* UUnitMainComponent::GetDamageReference(UUnitMainComponent* From)
{
	return DamageMap.Find(From);
}

float UUnitMainComponent::GetDamageValue_Implementation(UUnitMainComponent* From)
{
	if (float* DamageRef = GetDamageReference(From)) return *DamageRef;
	else return 0.0f;
}

float UUnitMainComponent::GetDamagePercent_Implementation(UUnitMainComponent* From)
{
	if (TotalTakeDamage <= 0.0f) return 0.0f;
	return GetDamageValue(From) / TotalTakeDamage;
}

float UUnitMainComponent::AddDamageValue_Implementation(UUnitMainComponent* From, float Value)
{
	float Result = Value;
	float* Origin = DamageMap.Find(From);
	if (Origin) Result += *Origin;
	DamageMap.Add(From, Result);
	TotalTakeDamage += Value;
	return Value;
}

void UUnitMainComponent::ResetDamageFrom_Implementation(UUnitMainComponent* From)
{
	if (float* DamageRef = GetDamageReference(From)) TotalTakeDamage -= *DamageRef;
	DamageMap.Remove(From);
}

void UUnitMainComponent::ResetDamageValue_Implementation()
{
	TotalTakeDamage = 0.0f;
	DamageMap.Empty();
}


void UUnitMainComponent::UnitMessage_Simple(const FName& Message)
{
	for (UUnitComponentBase* CurrentComponent : GetComponents())
	{
		if (!IsValid(CurrentComponent)) continue;
		CurrentComponent->ReceiveUnitMessage_Simple(Message);
	}
}

void UUnitMainComponent::UnitMessage_Detail(const FName& Message, const FName& Context)
{
	for (UUnitComponentBase* CurrentComponent : GetComponents())
	{
		if (!IsValid(CurrentComponent)) continue;
		CurrentComponent->ReceiveUnitMessage_Detail(Message, Context);
	}
}

void UUnitMainComponent::UnitMessage_Montage(UAnimMontage* Montage, bool bIsStart, bool bIsInterrupted)
{
	for (UUnitComponentBase* CurrentComponent : GetComponents())
	{
		if (!IsValid(CurrentComponent)) continue;
		CurrentComponent->ReceiveUnitMessage_Montage(Montage, bIsStart, bIsInterrupted);
	}
}


void UUnitMainComponent::MontageStarted(UAnimMontage* Montage)
{

}

void UUnitMainComponent::MontageEnded(UAnimMontage* Montage, bool bIsInterrupted)
{
	if (bIsInterrupted) return;
	if (MainActionMontageEvent.bIsStarted && Montage == MainActionMontageEvent.MontageToPlay)
	{
		StopMainActionMontage(false);
	}
}

void UUnitMainComponent::MontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	UnitMessage_Detail(UReservedActionMessage::GetActionMessage_MontageNotifyBegin(), NotifyName);
}

void UUnitMainComponent::MontageNotifyEnd(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	UnitMessage_Detail(UReservedActionMessage::GetActionMessage_MontageNotifyEnd(), NotifyName);
}


void UUnitMainComponent::Die_Implementation(const FDamageInfo& LastAttackDamageInfo)
{ 
	bIsDie = true;
	MainAction.Cancel();
	MainActionMontageEvent.Stop(AnimInstance);
	for (UActorComponent* CurrentComponent : GetComponents())
	{
		if (UUnitComponentBase* AsUnitComponent = Cast<UUnitComponentBase>(CurrentComponent)) AsUnitComponent->BroadcastMessage_Removed();
	}
	OnUnitDie.Broadcast(this, LastAttackDamageInfo);
	OnDie(LastAttackDamageInfo, DamageMap, TotalTakeDamage);
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


float UUnitMainComponent::TakeDamage_Implementation(const FDamageInfo& Info, bool& bIsKill)
{
	UUnitMainComponent* From = Info.DamageInstigator;
	if (!IDamageable::Execute_GetIsDamageable(this, From)) return 0.0f;
	FDamageInfo ResultInfo = Info;
	float& Result = ResultInfo.DamageValue;
	AddDamageValue(From, Result);
	if (UFillableValue* HPValue = FindFillValue(L"HP"))
	{
		Result = HPValue->AddValue(-Result);
		if (HPValue->GetIsEmpty()) Die(Info);
		else
		{
			OnUnitDamage.Broadcast(this, ResultInfo);
			OnTakeDamage(ResultInfo);
		}
	}
	else
	{
		OnUnitDamage.Broadcast(this, ResultInfo);
		OnTakeDamage(ResultInfo);
	}
	bIsKill = IDamageable::Execute_GetIsDie(this);
	return -Result;
}

bool UUnitMainComponent::GetIsAttackable_Implementation(UUnitMainComponent* From)
{
	if (!IsValid(From)) return false;
	if (!IDamageable::Execute_GetIsDamageable(this, From)) return false;
	return GetAllyTypeFromUnit(From) == EUnitAllyType::Enemy;
}

bool UUnitMainComponent::GetIsDamageable_Implementation(UUnitMainComponent* From)
{
	if (IDamageable::Execute_GetIsDie(this)) return false;
	return true;
}

bool UUnitMainComponent::GetIsDie_Implementation()
{
	if (bIsDie) return true;
	if (UFillableValue* HPValue = FindFillValue(L"HP")) return HPValue->GetIsEmpty();
	return false;
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

UUnitMainComponent* UUnitMainComponent::GetUnit(AActor* Target)
{
	if (IsValid(Target)) return Target->GetComponentByClass<UUnitMainComponent>();
	return nullptr;
}

bool UUnitMainComponent::TryGetUnit(AActor* Target, UUnitMainComponent*& OutResult)
{
	OutResult = GetUnit(Target);
	return OutResult != nullptr;
}