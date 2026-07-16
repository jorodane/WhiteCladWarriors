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
		if(ChaseLimitRange <= 0 || !FollowUntilChaseLimit_Implementation(CurrentAttackTarget))
		{
			bool bIsAttackable, bIsInRange;
			if (!TryAttack(CurrentAttackOperator, CurrentAttackTarget, bIsAttackable, bIsInRange) && !bIsAttackable) OnCannotAttackable();
		}
	}
	else if(ChaseLockTime < AMapSetting::GetCurrentWorldTime())
	{
		CurrentAttackTarget = GetDetectTarget(CurrentAttackOperator);
		OnAttackTargetDetected(CurrentAttackOperator, CurrentAttackTarget);
	}
}


bool UUnitAttackComponent::OnCannotAttackable_Implementation()
{
	switch (CurrentAttackMode)
	{
		case EAttackMode::Location:
		case EAttackMode::ReturnAfterTargetKill:
			ResetFocusTarget();
			MoveToClaimedLocation();
			break;
		default:
			ResetFocusTarget();
			MoveToChaseStartLocation();
			EndLastCursor();
			break;
	}
}

bool UUnitAttackComponent::OnAttackTargetDetected_Implementation(AOperator* Instigator, AActor* TargetActor)
{
	if (!IsValid(TargetActor)) return;
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
	default:
		CommandAttackTarget(Instigator, TargetActor);
		break;
	}
}


bool UUnitAttackComponent::FollowUntilChaseLimit_Implementation(AActor* Target)
{
	if (!IsValid(Target)) return;
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
	float CurrentDistance = FVector::Dist2D(Target->GetActorLocation(), LimitAnchor);

	if (ChaseLimitRange < CurrentDistance)
	{
		ChaseLockTime = AMapSetting::GetCurrentWorldTime() + ChaseDelay;
		ResetFocusTarget();
		return false;
	}
	else switch(CurrentAttackMode)
	{
		case EAttackMode::Location:
		case EAttackMode::ReturnAfterTargetKill:
			MoveToClaimedLocation();
			SetDetect(bIsDetectOnIdle);
			break;
	}
	return true;
}
