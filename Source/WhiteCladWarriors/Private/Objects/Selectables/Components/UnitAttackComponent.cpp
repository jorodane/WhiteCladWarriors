// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Selectables/Components/UnitAttackComponent.h"
#include "Objects/Selectables/Components/UnitMainComponent.h"
#include "Objects/Players/Operator.h"
#include "Objects/Values/ValueContainer.h"

#include "Actions/ActionBase.h"
#include "Actions/Executables/ActionExecutor.h"
#include "Actions/Executables/ActionNode.h"

#include "Settings/MapSetting.h"
#include "Kismet/GameplayStatics.h"

void UUnitAttackComponent::TickAttack_Implementation(float DeltaTime)
{
    if (!IsValid(OwnerUnit)) return;
    if (OwnerUnit->GetClass()->ImplementsInterface(UDamageable::StaticClass()) && IDamageable::Execute_GetIsDie(OwnerUnit)) return;
    switch (CurrentAttackMode)
    {
    case EAttackMode::Idle:
        if (bIsDetectOnIdle && !GetChaseLocked()) TickFindTarget(DeltaTime);
        break;
    case EAttackMode::Target:
        TickAttackTarget(DeltaTime);
        break;
    case EAttackMode::Location:
        TickAttackLocation(DeltaTime);
        break;
    }
}

bool UUnitAttackComponent::TickFindTarget_Implementation(float DeltaSeconds)
{
    AOperator* Operator = GetOperator();
    AActor* DetectTarget = GetDetectTarget(Operator);
    return OnAttackTargetDetected(DetectTarget);
}

void UUnitAttackComponent::TickAttackTarget_Implementation(float DeltaSeconds)
{
    if (bIsAttackExecuted) return;
    if (!GetValidAttackTarget())
    {
        OnCannotAttackable();
        return;
    }
    if(GetInRange(GetOwner(), AttackFocusTarget, GetAttackRange())) ExecuteAttack(AttackFocusTarget);
}

void UUnitAttackComponent::TickAttackLocation_Implementation(float DeltaSeconds)
{
    if (bIsAttackExecuted) return;
    if (GetValidAttackTarget() && GetInRange(GetOwner(), AttackFocusTarget, GetAttackRange())) ExecuteAttack(AttackFocusTarget);
    if (TickFindTarget(DeltaSeconds)) MoveToFocusTarget();
}


void UUnitAttackComponent::BeginAttackLocation_Implementation(FVector Target, const FActionCursorFinder& Cursor)
{
    CurrentAttackMode = EAttackMode::Location;
    AttackFocusLocation = Target;
    MoveToLocation(Target);
    ActionClaimer = Cursor;
}

void UUnitAttackComponent::BeginAttackTarget_Implementation(AActor* Target, const FActionCursorFinder& Cursor)
{
    CurrentAttackMode = EAttackMode::Target;
    AttackFocusTarget = Target;
    MoveToTarget(Target);
    ActionClaimer = Cursor;
}

void UUnitAttackComponent::ExecuteAttack_Implementation(AActor* Target)
{
    AActionBase* AttackAction = GetAttackAction();
    if (!IsValid(AttackAction) || !IsValid(Target)) { OnAttackTargetCompleted(); return; }
    UActionExecutor* ClaimExecutor = UActionExecutor::GetExecutorFromCursor(ActionClaimer);
    FOnNodeEnded NodeEndedDelegate;
    NodeEndedDelegate.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(UUnitAttackComponent, OnAttackActionCompleted));
    if (IsValid(ClaimExecutor))
    {
        UActionNode* ClaimNode = ClaimExecutor->GetNode(ActionClaimer);
        if (!IsValid(ClaimNode)) { OnAttackTargetCompleted(); return; }
        int ResultID = -1;
        AttackFocusTarget = Target;
        SetChaseLockTimeNow();

        ClaimExecutor->AddActor(L"AttackTarget", Target);
        UActionNode* ExecutedNode = ClaimExecutor->CreateSubNodeWithEvent(ActionClaimer, ClaimNode, AttackAction->RootAsSubNode, ResultID, NodeEndedDelegate);
        if(IsValid(ExecutedNode)) bIsAttackExecuted = true;
    }
    else
    {
        ClaimExecutor = AttackAction->ExecuteActionToTarget(GetOperator(), this, Target, FExecutorValueMap::Default);
        if (IsValid(ClaimExecutor))
        {
            ClaimExecutor->SetEndEventOnMainCursor(this, NodeEndedDelegate);
            bIsAttackExecuted = true;
        }
        else
        {
            OnAttackTargetCompleted();
            bIsAttackExecuted = false;
        }
    }
}

void UUnitAttackComponent::OnMainActionChanged_Implementation(const FMainActionInfo& ActionInfo, bool bIsValid)
{
    if (bIsValid)
    {
        UUnitActionComponent* ClaimComponent = ActionInfo.Cursor.CurrentComponent;
        bool ClaimBySelf = ClaimComponent == this;
        if(!ClaimBySelf) switch (CurrentAttackMode)
        {
        //case EAttackMode::Target:
        //case EAttackMode::Location:
        //    ResetDatas();
        //    break;
        case EAttackMode::Idle:
        case EAttackMode::Return:
            CurrentAttackMode = EAttackMode::Busy;
            break;
        }
    }
    else switch(CurrentAttackMode)
    {
    case EAttackMode::Target:
    case EAttackMode::Location:
        ResetDatas();
        ResetDetectionEnable();
        break;
    case EAttackMode::Busy:
    case EAttackMode::Return:
        CurrentAttackMode = EAttackMode::Idle;
        break;
    }
}

void UUnitAttackComponent::OnActorDetected_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int OtherBodyIndex, bool FromSweep, const FHitResult& SweepResult)
{
    if (!IsValid(OtherActor)) return;
    if (!OtherActor->GetClass()->ImplementsInterface(UDamageable::StaticClass())) return;
    DetectedActors.Add(OtherActor);
}

void UUnitAttackComponent::OnActorUndetected_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int OtherBodyIndex)
{
    DetectedActors.Remove(OtherActor);
}

bool UUnitAttackComponent::OnAttackTargetDetected_Implementation(AActor* TargetActor)
{
    if (!IsValid(TargetActor)) return false;
    SetChaseBeginning(GetLocation());
    BeginAttackTarget(TargetActor, FActionCursorFinder::None);
    return true;
}

void UUnitAttackComponent::OnAttackStop_Implementation()
{
    ResetDatas();
    SetChaseLockTimeNow();
}

void UUnitAttackComponent::OnAttackActionCompleted_Implementation(bool bIsCanceled)
{
    if (bIsCanceled)
    {
        OnAttackStop();
    }
    else
    {
        bIsAttackExecuted = false;
        if (GetValidAttackTarget()) MoveToFocusTarget();
        else OnCannotAttackable();
    }
}

void UUnitAttackComponent::OnAttackLocationCompleted_Implementation()
{
    bIsAttackExecuted = false;
    AttackFocusTarget = nullptr;
    AttackFocusLocation = FVector::ZeroVector;
    CurrentAttackMode = EAttackMode::Idle;
}

void UUnitAttackComponent::OnAttackTargetCompleted_Implementation()
{
    bIsAttackExecuted = false;
    AttackFocusTarget = nullptr;
    if (bIsReturnToOrigin && FVector::Dist2D(ChaseBeginLocation, GetLocation()) > 10.0f)
    {
        MoveToChaseBeginLocation();
        CurrentAttackMode = EAttackMode::Return;
    }
    else
    {
        CurrentAttackMode = EAttackMode::Idle;
    }
}

void UUnitAttackComponent::OnCannotAttackable_Implementation()
{
    switch (CurrentAttackMode)
    {
    case EAttackMode::Target:
        OnAttackTargetCompleted();
        break;
    case EAttackMode::Location:
        AttackFocusTarget = nullptr;
        MoveToFocusLocation();
        break;
    }
}

void UUnitAttackComponent::OnMoveCompleted_Implementation()
{
    switch (CurrentAttackMode)
    {
    case EAttackMode::Return:
        if (FVector::Dist2D(GetLocation(), ChaseBeginLocation) < 10.0f) CurrentAttackMode = EAttackMode::Idle;
        break;
    case EAttackMode::Location:
        //if (FVector::Dist2D(GetLocation(), AttackFocusLocation) < 10.0f) OnAttackLocationCompleted();
        break;
    }
}

void UUnitAttackComponent::OnUnitDamaged_Implementation(UUnitMainComponent* TargetUnit, const FDamageInfo& DamageInfo)
{
    if (!IsValid(TargetUnit)) return;
    if (!IsValid(DamageInfo.DamageCauser)) return;
    if (TargetUnit->HasMainAction()) return;

    OnDamageReaction(TargetUnit, DamageInfo);
}

void UUnitAttackComponent::OnDamageReaction_Implementation(UUnitMainComponent* TargetUnit, const FDamageInfo& DamageInfo)
{
    switch (CurrentAttackMode)
    {
    case EAttackMode::Idle: if (!bShouldCounterOnIdle) return;
        SetChaseBeginning(GetLocation());
        break;
    case EAttackMode::Return: if (!bShouldCounterOnIdle) return;
        break;
    default: return;
    }
    BeginAttackTarget(DamageInfo.DamageCauser, FActionCursorFinder::None);
}

void UUnitAttackComponent::OnUnitDied_Implementation(UUnitMainComponent* TargetUnit, const FDamageInfo& DamageInfo)
{
    OnAttackStop();
    BroadcastMessage_Simple(L"AttackFailed");
}


AActor* UUnitAttackComponent::GetDetectTarget_Implementation(AOperator* Operator)
{
    if (DetectedActors.Num() == 0) return nullptr;
    if (!IsValid(OwnerUnit)) return nullptr;
    AActor* OwnActor = GetOwner();
    if (!IsValid(OwnActor)) return nullptr;
    AActor* Result = nullptr;
    float NearDistance = DetectRange;
    for (AActor* CurrentActor : DetectedActors)
    {
        if (!GetAttackable(CurrentActor)) continue;
        float CurrentDistance = OwnActor->GetHorizontalDistanceTo(CurrentActor);
        if (!GetInRange(OwnActor, CurrentActor, NearDistance)) continue;
        NearDistance = CurrentDistance;
        Result = CurrentActor;
    }
    return Result;
}

TArray<AActor*> UUnitAttackComponent::GetDetectTargets_Implementation()
{
    return DetectedActors.Array();
}

bool UUnitAttackComponent::GetAttackable_Implementation(AActor* Target)
{
    if (!IsValid(Target)) return false;
    if (!Target->GetClass()->ImplementsInterface(UDamageable::StaticClass()) || !IDamageable::Execute_GetIsAttackable(Target, OwnerUnit)) return false;
    return true;
}

bool UUnitAttackComponent::GetInRange_Implementation(AActor* From, AActor* Target, float WantRange)
{
    if (!IsValid(From) || !IsValid(Target)) return false;
    float CurrentDistance = From->GetHorizontalDistanceTo(Target);
    return CurrentDistance < WantRange;
}

float UUnitAttackComponent::GetAttackRange_Implementation()
{
    UValueContainer* Container = UValueContainer::GetValueContainer(GetOwner());
    if (!IsValid(Container)) return AttackRange;
    return Container->GetNumber("AttackRange", AttackRange);
}

void UUnitAttackComponent::ResetDetectionEnable_Implementation()
{
    SetDetectionEnable(bIsDetectOnIdle);
    if (!bIsDetectOnIdle) DetectedActors.Empty();
}

void UUnitAttackComponent::ResetDatas_Implementation()
{
    CurrentAttackMode = EAttackMode::Idle;
    AttackFocusLocation = FVector::ZeroVector;
    AttackFocusTarget = nullptr;
    bIsAttackExecuted = false;
    bIsReturnToOrigin = false;
}

void UUnitAttackComponent::SetChaseBeginning_Implementation(FVector WantLocation)
{
    ChaseBeginLocation = WantLocation;
    bIsReturnToOrigin = true;
}

void UUnitAttackComponent::SetChaseLockTime_Implementation(float WantTime)
{
    ChaseLockTime = WantTime + ChaseDelay;
}
void UUnitAttackComponent::SetChaseLockTimeNow_Implementation()
{
    SetChaseLockTime(AMapSetting::GetCurrentWorldTime());
}
bool UUnitAttackComponent::GetChaseLocked_Implementation()
{
    return AMapSetting::GetCurrentWorldTime() < ChaseLockTime;
}


void UUnitAttackComponent::OnPoolEnqueue_Implementation(UPoolComponent* EnqueueTo)
{
    OnAttackStop();
    SetComponentTickEnabled(false);
}

void UUnitAttackComponent::OnPoolDequeue_Implementation(UPoolComponent* DequeueFrom)
{
    SetComponentTickEnabled(true);
}