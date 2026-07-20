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

void UUnitAttackComponent::TickAttackCheck_Implementation(float DeltaSeconds)
{
    if (CurrentAttackMode == EAttackMode::Idle) return;
    if (!IsValid(OwnerUnit)) return;
    if (OwnerUnit->GetClass()->ImplementsInterface(UDamageable::StaticClass()) && IDamageable::Execute_GetIsDie(OwnerUnit)) return;
    if (OwnerUnit->HasMainAction()) return;

    AActor* CurrentAttackTarget = AttackFocusTarget;
    AOperator* CurrentAttackOperator = LastClaimOperator;
    if (IsValid(CurrentAttackTarget)) TickAttackTarget(CurrentAttackOperator, CurrentAttackTarget);
    else TickSearchTarget(CurrentAttackOperator, CurrentAttackTarget);
}

void UUnitAttackComponent::TickAttackTarget_Implementation(AOperator* CurrentAttackOperator, AActor* CurrentAttackTarget)
{
    if (IsValid(CurrentAttackTarget))
    {
        const bool bCanContinueChasing = ChaseLimitRange <= 0.0f || FollowUntilChaseLimit(CurrentAttackTarget);
        if (!bCanContinueChasing) return;

        bool bIsAttackable, bIsInRange;
        const bool bAttackSucceeded = TryAttack(CurrentAttackOperator, CurrentAttackTarget, bIsAttackable, bIsInRange);

        if (!bAttackSucceeded && (!bIsAttackable || (bIsAttackable && bIsInRange))) OnCannotAttackable();
    }
}

void UUnitAttackComponent::TickSearchTarget_Implementation(AOperator* CurrentAttackOperator, AActor* CurrentAttackTarget)
{
    if (ChaseLockTime < AMapSetting::GetCurrentWorldTime())
    {
        CurrentAttackTarget = GetDetectTarget(CurrentAttackOperator);
        OnAttackTargetDetected(CurrentAttackOperator, CurrentAttackTarget);
    }
}

void UUnitAttackComponent::BeginAttackLocation_Implementation(FVector Target, const FActionCursorFinder& Cursor, float ChaseRange)
{
    EndLastAction();
    ChaseLimitRange = ChaseRange;
    bIsReturnToOrigin = false;
    ActionClaimer = Cursor;
    AttackFocusLocation = Target;
    SetAttackMode(EAttackMode::Location);
    SetDetectionEnable(true);
    MoveToClaimedLocation();
}

void UUnitAttackComponent::BeginAttackTarget_Implementation(AActor* Target, const FActionCursorFinder& Cursor, float ChaseRange)
{
    EndLastAction();
    bIsReturnToOrigin = ChaseRange > 0;
    ChaseLimitRange = ChaseRange;
    ActionClaimer = Cursor;
    RefreshChaseBeginLocation();
    SetFocusTarget(Cursor.CurrentOperator, Target);
    SetAttackMode(EAttackMode::Target);
    SetDetectionEnable(false);
    MoveToFocusTarget();
}


bool UUnitAttackComponent::FollowUntilChaseLimit_Implementation(AActor* Target)
{
    if (!IsValid(Target)) return false;

    FVector LimitAnchor;
    switch (CurrentAttackMode)
    {
    case EAttackMode::Location:
        LimitAnchor = AttackFocusLocation;
        break;
    default:
        LimitAnchor = ChaseBeginLocation;
        break;
    }

    const float CurrentDistance = FVector::Dist2D(Target->GetActorLocation(), LimitAnchor);
    if (CurrentDistance <= ChaseLimitRange) return true;

    ChaseLockTime = AMapSetting::GetCurrentWorldTime() + ChaseDelay;
    ResetFocusTarget();
    if (CurrentAttackMode == EAttackMode::Location)
    {
        MoveToClaimedLocation();
        SetDetectionEnable(true);
    }
    else if (bIsReturnToOrigin)
    {
        MoveToClaimedLocation();
        SetDetectionEnable(bIsDetectOnIdle);
    }
    else
    {
        MoveToChaseBeginLocation();
        EndLastAction();
    }

    return false;
}

bool UUnitAttackComponent::OnCannotAttackable_Implementation()
{
    if (CurrentAttackMode == EAttackMode::Location || bIsReturnToOrigin)
    {
        MoveToClaimedLocation();
    }
    else
    {
        MoveToChaseBeginLocation();
    }
    EndLastAction();
    ResetFocusTarget();
    return true;
}

void UUnitAttackComponent::OnActorDetected_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int OtherBodyIndex, bool FromSweep, const FHitResult& SweepResult)
{
    if (!IsValid(OtherActor)) return;
    if (!OwnerUnit->GetClass()->ImplementsInterface(UDamageable::StaticClass()) || !IDamageable::Execute_GetIsAttackable(OwnerUnit, OwnerUnit)) return;
    DetectedActors.Add(OtherActor);
}

void UUnitAttackComponent::OnActorUndetected_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int OtherBodyIndex)
{
    DetectedActors.Remove(OtherActor);
}


bool UUnitAttackComponent::OnAttackTargetDetected_Implementation(AOperator* Instigator, AActor* TargetActor)
{
    if (!IsValid(TargetActor)) return false;
    if (!IsValid(Instigator) && IsValid(OwnerUnit)) Instigator = OwnerUnit->GetOperator();

    switch (CurrentAttackMode)
    {
    case EAttackMode::Location:
        RefreshChaseBeginLocation();
        SetFocusTarget(Instigator, TargetActor);
        MoveToFocusTarget();
        return true;

    case EAttackMode::FindTarget:
        RefreshChaseBeginLocation();
        SetFocusTarget(Instigator, TargetActor);
        [[fallthrough]];

    default:
        return CommandAttackTarget(Instigator, TargetActor);
    }
}

void UUnitAttackComponent::OnAttackTargetCompleted_Implementation()
{
    MoveToClaimedLocation();
}

void UUnitAttackComponent::OnMoveCompleted_Implementation()
{
    if (!IsValid(AttackFocusTarget) && CurrentAttackMode == EAttackMode::Location) ResetAttackDatas();
}

void UUnitAttackComponent::OnUnitDied_Implementation(UUnitMainComponent* TargetUnit, const FDamageInfo& DamageInfo)
{
    ResetAttackDatas();
}

void UUnitAttackComponent::OnMainActionChanged_Implementation(const FMainActionInfo& NewMainAction, bool bIsValid)
{
    if (!IsValid(OwnerUnit)) return;
    if (OwnerUnit->GetClass()->ImplementsInterface(UDamageable::StaticClass()) && IDamageable::Execute_GetIsDie(OwnerUnit)) return;
    if (!bIsValid) ResetAttackDatas();
}

void UUnitAttackComponent::SetFocusTarget_Implementation(AOperator* Instigator, AActor* TargetActor)
{
    LastClaimOperator = Instigator;
    AttackFocusTarget = TargetActor;
}

void UUnitAttackComponent::ResetFocusTarget_Implementation()
{
    SetFocusTarget(nullptr, nullptr);
}

void UUnitAttackComponent::ResetAttackDatas_Implementation()
{
    ResetAttackMode();
    ResetFocusTarget();
    EndLastAction();
    AttackFocusLocation = FVector::ZeroVector;
    ChaseLimitRange = 0.0f;
    ChaseLockTime = 0.0f;
}

void UUnitAttackComponent::ResetAttackMode_Implementation()
{
    bIsReturnToOrigin = false;
    SetAttackMode(bIsDetectOnIdle ? EAttackMode::FindTarget : EAttackMode::Idle);
    if (bIsDetectOnIdle) SetDetectionEnable(true);
}

void UUnitAttackComponent::SetAttackMode_Implementation(const EAttackMode NewMode)
{
    CurrentAttackMode = NewMode;
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

void UUnitAttackComponent::SetChaseBeginLocation_Implementation(FVector NewLocation)
{
    ChaseBeginLocation = NewLocation;
}

FVector UUnitAttackComponent::GetCurrentLocation_Implementation()
{
    AActor* OwnerActor = GetOwner();
    if (IsValid(OwnerActor)) return OwnerActor->GetActorLocation();
    return FVector::ZeroVector;
}

void UUnitAttackComponent::MoveToClaimedLocation_Implementation()
{
    if (IsValid(AttackFocusTarget)) MoveToFocusTarget();
    else if (CurrentAttackMode == EAttackMode::Location) MoveToFocusLocation();
    else if (bIsReturnToOrigin) MoveToChaseBeginLocation();
}

void UUnitAttackComponent::EndLastAction_Implementation()
{
    UActionExecutor* ClaimExecutor = UActionExecutor::GetExecutorFromCursor(ActionClaimer);
    if (IsValid(ClaimExecutor)) ClaimExecutor->EndNode(ActionClaimer, UActionExecutor::GetNodeFromCursor(ActionClaimer), true);
    ActionClaimer.Clear();
}

bool UUnitAttackComponent::TryAttack_Implementation(AOperator* ClaimOperator, AActor* WantTarget, bool& outIsAttackable, bool& outIsInRange)
{
    outIsAttackable = outIsInRange = false;
    if (!IsValid(WantTarget)) return false;
    if (!IsValid(OwnerUnit)) return false;
    outIsAttackable = GetAttackable(WantTarget);
    outIsInRange = GetInRange(GetOwner(), WantTarget, GetAttackRange());
    if (!outIsAttackable || !outIsInRange) return false;
    return CommandAttackTarget(ClaimOperator, WantTarget);
}

bool UUnitAttackComponent::CommandAttackTarget(AOperator* Operator, AActor* Target, AActionBase* AttackAction)
{
    UActionExecutor* ClaimExecutor = UActionExecutor::GetExecutorFromCursor(ActionClaimer);
    if (!IsValid(AttackAction) || !IsValid(ClaimExecutor) || !IsValid(Target)) { ResetAttackDatas(); return false; }
    ClaimExecutor->AddActor(L"AttackTarget", Target);
    UActionNode* ClaimNode = ClaimExecutor->GetNode(ActionClaimer);
    if(!IsValid(ClaimNode)) { ResetAttackDatas(); return false; }
    int ResultID = -1;
    FOnNodeEnded NodeEndedDelegate;
    NodeEndedDelegate.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(UUnitAttackComponent, OnAttackTargetCompleted));
    return IsValid(ClaimExecutor->CreateSubNodeWithEvent(ActionClaimer, ClaimNode, AttackAction->RootAsSubNode, ResultID, NodeEndedDelegate));
}

bool UUnitAttackComponent::CommandAttackTarget_Implementation(AOperator* Operator, AActor* Target)
{
    AActionBase* AttackAction = GetAttackAction();
    if (!IsValid(AttackAction)) { ResetAttackDatas(); return false; }
    return CommandAttackTarget(Operator, Target, AttackAction);
}

void UUnitAttackComponent::CommandChaseTarget_Implementation(AOperator* Operator, AActor* Target)
{
    AActionBase* ChaseAction = GetChaseAction();
    if (!IsValid(ChaseAction)) { ResetAttackDatas(); return; }
    ChaseAction->ExecuteActionToTarget(Operator, this, Target, FExecutorValueMap::Default);
    return;
}

void UUnitAttackComponent::OnPoolEnqueue_Implementation(UPoolComponent* EnqueueTo)
{
    SetComponentTickEnabled(false);
    ResetAttackDatas();
}

void UUnitAttackComponent::OnPoolDequeue_Implementation(UPoolComponent* DequeueFrom)
{
    SetComponentTickEnabled(true);
}