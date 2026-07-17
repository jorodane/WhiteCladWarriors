// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Selectables/Components/UnitAttackComponent.h"
#include "Objects/Selectables/Components/UnitMainComponent.h"
#include "Objects/Players/Operator.h"
#include "Settings/MapSetting.h"
#include "Kismet/GameplayStatics.h"

void UUnitAttackComponent::TickAttackCheck_Implementation(float DeltaSeconds)
{
    AActor* CurrentAttackTarget = AttackFocusTarget;
    AOperator* CurrentAttackOperator = LastClaimOperator;

    if (IsValid(CurrentAttackTarget))
    {
        const bool bCanContinueChasing = ChaseLimitRange <= 0.0f || FollowUntilChaseLimit_Implementation(CurrentAttackTarget);
        if (!bCanContinueChasing) return;

        bool bIsAttackable, bIsInRange;
        const bool bAttackSucceeded = TryAttack(CurrentAttackOperator, CurrentAttackTarget, bIsAttackable, bIsInRange);

        if (!bAttackSucceeded && !bIsAttackable) OnCannotAttackable();
        return;
    }

    if (ChaseLockTime < AMapSetting::GetCurrentWorldTime())
    {
        CurrentAttackTarget = GetDetectTarget(CurrentAttackOperator);
        OnAttackTargetDetected(CurrentAttackOperator,CurrentAttackTarget);
    }
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
        LimitAnchor = ChaseStartLocation;
        break;
    }

    const float CurrentDistance = FVector::Dist2D(Target->GetActorLocation(), LimitAnchor);
    if (CurrentDistance <= ChaseLimitRange) return true;

    ChaseLockTime = AMapSetting::GetCurrentWorldTime() + ChaseDelay;
    ResetFocusTarget();
    switch (CurrentAttackMode)
    {
    case EAttackMode::Location:
    case EAttackMode::ReturnAfterTargetKill:
        MoveToClaimedLocation();
        SetDetect(bIsDetectOnIdle);
        break;

    default:
        MoveToChaseStartLocation();
        break;
    }

    return false;
}

bool UUnitAttackComponent::OnCannotAttackable_Implementation()
{
    ResetFocusTarget();

    switch (CurrentAttackMode)
    {
    case EAttackMode::Location:
    case EAttackMode::ReturnAfterTargetKill:
        MoveToClaimedLocation();
        break;

    default:
        MoveToChaseStartLocation();
        EndLastCursor();
        break;
    }

    return true;
}

bool UUnitAttackComponent::OnAttackTargetDetected_Implementation(AOperator* Instigator, AActor* TargetActor)
{
    if (!IsValid(TargetActor)) return false;
    if (!IsValid(Instigator) && IsValid(OwnerUnit)) Instigator = OwnerUnit->GetOperator();

    switch (CurrentAttackMode)
    {
    case EAttackMode::Location:
        RefreshChaseStartLocation();
        SetFocusTarget(Instigator, TargetActor);
        MoveToFocusTarget();
        break;

    case EAttackMode::FindTarget:
        RefreshChaseStartLocation();
        [[fallthrough]];

    default:
        CommandAttackTarget(Instigator, TargetActor);
        break;
    }

    return true;
}

void UUnitAttackComponent::SetFocusTarget_Implementation(AOperator* Instigator, AActor* TargetActor)
{

}

void UUnitAttackComponent::ResetFocusTarget_Implementation()
{

}

void UUnitAttackComponent::SetDetect_Implementation(bool bWantDetectAround)
{

}

AActor* UUnitAttackComponent::GetDetectTarget_Implementation(AOperator* Operator)
{
    return nullptr;
}

void UUnitAttackComponent::SetChaseStartLocation_Implementation(FVector NewLocation)
{

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
    else switch (CurrentAttackMode)
    {
         
    };
}

void UUnitAttackComponent::EndLastCursor_Implementation()
{

}

bool UUnitAttackComponent::TryAttack_Implementation(AOperator* ClaimOperator, AActor* WantTarget, bool& outIsAttackable, bool& outIsInRange)
{
    return false;
}

void UUnitAttackComponent::CommandAttackTarget_Implementation(AOperator* Operator, AActor* Target)
{

}

