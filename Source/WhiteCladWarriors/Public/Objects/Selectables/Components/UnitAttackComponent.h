// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/Selectables/Components/UnitActionComponent.h"
#include "Interfaces/Poolable.h"
#include "Generals/Structs/DamageStructures.h"
#include "Generals/Structs/ActionStructures.h"
#include "UnitAttackComponent.generated.h"

class AActionBase;
class AOperator;

UENUM(BlueprintType)
enum class EAttackMode : uint8
{
	Idle, Return, Location, Target
};

/**
 * 
 */
UCLASS()
class WHITECLADWARRIORS_API UUnitAttackComponent : public UUnitActionComponent, public IPoolable
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	EAttackMode CurrentAttackMode;
	
	UPROPERTY(BlueprintReadOnly, Category = "Attack")
	TObjectPtr<AOperator> LastClaimOperator;

	UPROPERTY(BlueprintReadOnly, Category = "Attack")
	TObjectPtr<AActor> AttackFocusTarget;

	TSet<AActor*> DetectedActors;

	UPROPERTY(BlueprintReadOnly, Category = "Attack")
	FVector AttackFocusLocation;

	UPROPERTY(BlueprintReadOnly, Category = "Attack")
	FVector ChaseBeginLocation;

	UPROPERTY(BlueprintReadOnly, Category = "Attack")
	FActionCursorFinder ActionClaimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	float DetectRange = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	float AttackRange = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	float ChaseLimitRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	float ChaseLockTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	float ChaseDelay = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	bool bIsDetectOnIdle = true;

	UPROPERTY(BlueprintReadOnly, Category = "Attack")
	bool bIsReturnToOrigin = false;

	UPROPERTY(BlueprintReadOnly, Category = "Attack")
	bool bIsAttackExecuted = false;

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void TickAttack(float DeltaTime);
	void TickAttack_Implementation
(float DeltaTime);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	bool TickFindTarget(float DeltaSeconds);
	bool TickFindTarget_Implementation(float DeltaSeconds);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void TickAttackTarget(float DeltaSeconds);
	void TickAttackTarget_Implementation(float DeltaSeconds);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void TickAttackLocation(float DeltaSeconds);
	void TickAttackLocation_Implementation(float DeltaSeconds);

	/*
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void TickAttackCheck(float DeltaSeconds);
	void TickAttackCheck_Implementation(float DeltaSeconds);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void TickAttackTarget(AOperator* CurrentAttackOperator, AActor* CurrentAttackTarget);
	void TickAttackTarget_Implementation(AOperator* CurrentAttackOperator, AActor* CurrentAttackTarget);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void TickSearchTarget(AOperator* CurrentAttackOperator, AActor* CurrentAttackTarget);
	void TickSearchTarget_Implementation(AOperator* CurrentAttackOperator, AActor* CurrentAttackTarget);



	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	bool FollowUntilChaseLimit(AActor* Target);
	bool FollowUntilChaseLimit_Implementation(AActor* Target);
	

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void SetFocusTarget(AOperator* Instigator, AActor* TargetActor);
	void SetFocusTarget_Implementation(AOperator* Instigator, AActor* TargetActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void ResetFocusTarget();
	void ResetFocusTarget_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void ResetAttackDatas();
	void ResetAttackDatas_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void ResetAttackMode();
	void ResetAttackMode_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void SetAttackMode(const EAttackMode NewMode);
	void SetAttackMode_Implementation(const EAttackMode NewMode);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void SetChaseBeginLocation(FVector NewLocation);
	void SetChaseBeginLocation_Implementation(FVector NewLocation);

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Attack")
	FVector GetCurrentLocation();
	FVector GetCurrentLocation_Implementation();

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void RefreshChaseBeginLocation() { SetChaseBeginLocation(GetCurrentLocation()); }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void MoveToClaimedLocation();
	void MoveToClaimedLocation_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void EndLastAction();
	void EndLastAction_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	bool TryAttack(AOperator* ClaimOperator, AActor* WantTarget, bool& outIsAttackable, bool& outIsInRange);
	bool TryAttack_Implementation(AOperator* ClaimOperator, AActor* WantTarget, bool& outIsAttackable, bool& outIsInRange);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	bool CommandAttackTarget(AOperator* Operator, AActor* Target);
	bool CommandAttackTarget_Implementation(AOperator* Operator, AActor* Target);
	bool CommandAttackTarget(AOperator* Operator, AActor* Target, AActionBase* AttackAction);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	bool CommandChaseTarget(AOperator* Operator, AActor* Target);
	bool CommandChaseTarget_Implementation(AOperator* Operator, AActor* Target);
	*/

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void BeginAttackLocation(FVector Target, const FActionCursorFinder& Cursor, float ChaseRange);
	void BeginAttackLocation_Implementation(FVector Target, const FActionCursorFinder& Cursor, float ChaseRange);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void BeginAttackTarget(AActor* Target, const FActionCursorFinder& Cursor, float ChaseRange);
	void BeginAttackTarget_Implementation(AActor* Target, const FActionCursorFinder& Cursor, float ChaseRange);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void ExecuteAttack(AActor* Target);
	void ExecuteAttack_Implementation(AActor* Target);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void OnActorDetected(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int OtherBodyIndex, bool FromSweep, const FHitResult& SweepResult);
	void OnActorDetected_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int OtherBodyIndex, bool FromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void OnActorUndetected(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int OtherBodyIndex);
	void OnActorUndetected_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int OtherBodyIndex);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	bool OnAttackTargetDetected(AOperator* Instigator, AActor* TargetActor);
	bool OnAttackTargetDetected_Implementation(AOperator* Instigator, AActor* TargetActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void OnAttackStop();
	void OnAttackStop_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void OnAttackActionCompleted();
	void OnAttackActionCompleted_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void OnAttackTargetCompleted();
	void OnAttackTargetCompleted_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void OnAttackLocationCompleted();
	void OnAttackLocationCompleted_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void OnCannotAttackable();
	void OnCannotAttackable_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void OnMoveCompleted();
	void OnMoveCompleted_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void OnUnitDamaged(UUnitMainComponent* TargetUnit, const FDamageInfo& DamageInfo);
	void OnUnitDamaged_Implementation(UUnitMainComponent* TargetUnit, const FDamageInfo& DamageInfo);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void OnDamageReaction(UUnitMainComponent* TargetUnit, const FDamageInfo& DamageInfo);
	void OnDamageReaction_Implementation(UUnitMainComponent* TargetUnit, const FDamageInfo& DamageInfo);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void OnUnitDied(UUnitMainComponent* TargetUnit, const FDamageInfo& DamageInfo);
	void OnUnitDied_Implementation(UUnitMainComponent* TargetUnit, const FDamageInfo& DamageInfo);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	AActor* GetDetectTarget(AOperator* Operator);
	AActor* GetDetectTarget_Implementation(AOperator* Operator);

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Attack")
	TArray<AActor*> GetDetectTargets();
	TArray<AActor*> GetDetectTargets_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Attack")
	bool GetAttackable(AActor* Target);
	bool GetAttackable_Implementation(AActor* Target);

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Attack")
	bool GetValidAttackTarget();
	bool GetValidAttackTarget_Implementation() { return GetAttackable(AttackFocusTarget); }

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Attack")
	bool GetInRange(AActor* From, AActor* Target, float WantRange);
	bool GetInRange_Implementation(AActor* From, AActor* Target, float WantRange);

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Attack")
	float GetAttackRange();
	float GetAttackRange_Implementation();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Attack")
	void MoveToLocation(FVector WantLocation);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Attack")
	void MoveToTarget(AActor* WantTarget);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void MoveToFocusTarget();
	void MoveToFocusTarget_Implementation() { if (IsValid(AttackFocusTarget)) MoveToTarget(AttackFocusTarget); }


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void MoveToFocusLocation();
	void MoveToFocusLocation_Implementation() { MoveToLocation(AttackFocusLocation); }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void MoveToChaseBeginLocation();
	void MoveToChaseBeginLocation_Implementation() { MoveToLocation(ChaseBeginLocation); }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void ResetDetectionEnable();
	void ResetDetectionEnable_Implementation();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Attack")
	void SetDetectionEnable(bool bWantDetectAround);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Attack")
	bool GetDetectionEnable();

	UFUNCTION(BlueprintImplementableEvent, BlueprintPure, Category = "Attack")
	AActionBase* GetChaseAction();

	UFUNCTION(BlueprintImplementableEvent, BlueprintPure, Category = "Attack")
	AActionBase* GetAttackAction();

	virtual void OnPoolEnqueue_Implementation(UPoolComponent* EnqueueTo) override;
	virtual void OnPoolDequeue_Implementation(UPoolComponent* DequeueFrom) override;

};
