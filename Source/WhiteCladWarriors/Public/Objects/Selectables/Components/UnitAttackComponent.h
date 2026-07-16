// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/Selectables/Components/UnitActionComponent.h"
#include "UnitAttackComponent.generated.h"

class AOperator;

UENUM(BlueprintType)
enum class EAttackMode : uint8
{
	Idle, FindTarget, Location, Target, ReturnAfterTargetKill
};

/**
 * 
 */
UCLASS()
class WHITECLADWARRIORS_API UUnitAttackComponent : public UUnitActionComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	EAttackMode CurrentAttackMode;
	
	UPROPERTY(BlueprintReadOnly, Category = "Attack")
	TObjectPtr<AOperator> LastClaimOperator;

	UPROPERTY(BlueprintReadOnly, Category = "Attack")
	TObjectPtr<AActor> AttackFocusTarget;

	UPROPERTY(BlueprintReadOnly, Category = "Attack")
	FVector AttackFocusLocation;

	UPROPERTY(BlueprintReadOnly, Category = "Attack")
	FVector ChaseStartLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	float ChaseLimitRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	float ChaseLockTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	float ChaseDelay = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	bool bIsDetectOnIdle = true;

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void TickAttackCheck(float DeltaSeconds);
	void TickAttackCheck_Implementation(float DeltaSeconds);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	bool FollowUntilChaseLimit(AActor* Target);
	bool FollowUntilChaseLimit_Implementation(AActor* Target);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	bool OnCannotAttackable();
	bool OnCannotAttackable_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	bool OnAttackTargetDetected(AOperator* Instigator, AActor* TargetActor);
	bool OnAttackTargetDetected_Implementation(AOperator* Instigator, AActor* TargetActor);


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void SetFocusTarget(AOperator* Instigator, AActor* TargetActor);
	void SetFocusTarget_Implementation(AOperator* Instigator, AActor* TargetActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void ResetFocusTarget();
	void ResetFocusTarget_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void SetDetect(bool bWantDetectAround);
	void SetDetect_Implementation(bool bWantDetectAround);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	AActor* GetDetectTarget(AOperator* Operator);
	AActor* GetDetectTarget_Implementation(AOperator* Operator);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void SetChaseStartLocation(FVector NewLocation);
	void SetChaseStartLocation_Implementation(FVector NewLocation);

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Attack")
	FVector GetCurrentLocation();
	FVector GetCurrentLocation_Implementation();

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void RefreshChaseStartLocation() { SetChaseStartLocation(GetCurrentLocation()); }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void MoveToFocusTarget();
	void MoveToFocusTarget_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void MoveToClaimedLocation();
	void MoveToClaimedLocation_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void MoveToChaseStartLocation();
	void MoveToChaseStartLocation_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void EndLastCursor();
	void EndLastCursor_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	bool TryAttack(AOperator* ClaimOperator, AActor* WantTarget, bool& outIsAttackable, bool& outIsInRange);
	bool TryAttack_Implementation(AOperator* ClaimOperator, AActor* WantTarget, bool& outIsAttackable, bool& outIsInRange);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void CommandAttackTarget(AOperator* Operator, AActor* Target);
	void CommandAttackTarget_Implementation(AOperator* Operator, AActor* Target);

};
