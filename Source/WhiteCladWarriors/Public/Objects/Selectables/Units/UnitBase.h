// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Generals/Structs/InputPackage.h"
#include "GameFramework/Character.h"
#include "Styling/SlateBrush.h"
#include "Interfaces/Selectable.h"
#include "UnitBase.generated.h"

class AActionBase;
class UUnitActionComponent;
class UActionExecutor;
class UActionNode;
struct FInputPackage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnitDie, AUnitBase*, TargetUnit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnMovementStart, const FVector&, Destination, AActor*, TargetActor, float, AcceptanceRadius, UActionExecutor*, Executor, UUnitActionComponent*, TargetComponent, int, ID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMovementStop);

DECLARE_DYNAMIC_DELEGATE_FourParams(FOnMontageNotify, UActionExecutor*, Executor, UUnitActionComponent*, TargetComponent, int, ID, FName, NotifyName);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnMontageStart, UActionExecutor*, Executor, UUnitActionComponent*, TargetComponent, int, ID);
DECLARE_DYNAMIC_DELEGATE_FourParams(FOnMontageEnd, UActionExecutor*, Executor, UUnitActionComponent*, TargetComponent, int, ID, bool, bIsInterrupted);

USTRUCT(BlueprintType)
struct FActionReservator
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	TObjectPtr<AOperator> Operator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	TObjectPtr<AActionBase> Action;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	TObjectPtr<UActionExecutor> Executor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	TArray<UUnitActionComponent*> RunningComponents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	FInputPackage Input;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Valid")
	bool bIsValid = false;

	FActionReservator() {};
	FActionReservator(AOperator* TargetOperator, AActionBase* TargetAction, const FInputPackage& TargetInput)
	{
		Operator = TargetOperator;
		Action = TargetAction;
		Input = TargetInput;
	}

	bool Run(TArray<UUnitActionComponent*> StartComponents);
	bool SetEnd(UActionExecutor* EndExecutor, UUnitActionComponent* EndComponent);
};

USTRUCT(BlueprintType)
struct FMontageEventInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	TObjectPtr<UActionExecutor> MontageExecutor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	TObjectPtr<UUnitActionComponent> MontageComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	int RequestedID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	UAnimMontage* MontageToPlay;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	float PlayRate;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	float StartingPosition;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	FOnMontageNotify OnMontageNotifyBegin;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	FOnMontageNotify OnMontageNotifyEnd;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	FOnMontageStart OnMontageStart;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	FOnMontageEnd OnMontageEnd;

	bool bIsStarted = false;

public:
	bool ValidExecutor() const;
	void Clear() {MontageExecutor = nullptr; MontageComponent = nullptr; bIsStarted = false;}
	void MontageNotifyBegin(FName NotifyName);
	void MontageNotifyEnd(FName NotifyName);
	void MontageStart();
	void MontageEnd(bool bIsInterrupted);
};

USTRUCT(BlueprintType)
struct FMainActionInfo
{
	GENERATED_BODY()

	TWeakObjectPtr<UActionExecutor> Executor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	TWeakObjectPtr<UUnitActionComponent> Component;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	bool bIsCancelable = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	bool bIsStopMovement = false;

	void Clear();

	void Set(UActionExecutor* WantExecutor, UUnitActionComponent* WantComponent, bool bWantIsCancelable = true, bool bWantIsStopMovement = false);

	void Clear(const UActionExecutor* OldExecutor);

	void SetActionMessage_Simple(FName Message);

	bool Cancel(bool bWantStopMovement);

	void End(bool bWantStopMovement);

	bool IsValid() const;
};

UCLASS()
class WHITECLADWARRIORS_API AUnitBase : public ACharacter, public ISelectable
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Select")
	FOnUnitDie OnUnitDie;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Select")
	FOnMovementStop OnMovementStop;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Select")
	FOnMovementStart OnMovementStart;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Select")
	FSlateBrush SelectedIcon;

	TMap<AActionBase*, TArray<UUnitActionComponent*>>  ActionMap;

	TQueue<FActionReservator> ActionQueue;

	FActionReservator CurrentReservatedAction;

	UPROPERTY(BlueprintReadOnly, Category = "Select")
	TArray<UUnitActionComponent*> ActionComponentArray;

	FMainActionInfo MainAction;

	FMontageEventInfo ClaimedMontageEvent;
	FMontageEventInfo QueuedMontageEvent;

protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

public:
	UFUNCTION(BlueprintPure, Category = "Action")
	TArray<AActionBase*> GetActionList() const;

	UFUNCTION(BlueprintPure, Category = "Action")
	TArray<UUnitActionComponent*> GetComponentsWithAction(AActionBase* TargetAction) const;

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool GetSimpleAction(const FInputPackage& CurrentInput, AActionBase*& OutAction, TArray<UUnitActionComponent*>& OutComponents) const;

	UFUNCTION(BlueprintCallable, Category = "Action")
	void AddActionComponent(UUnitActionComponent* NewComponent);

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Action")
	bool GetMainActionCancelable() const;
	virtual bool GetMainActionCancelable_Implementation() const;

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool SetMainAction(const FMainActionInfo& Info);
	bool SetMainAction(UActionExecutor* Executor, UUnitActionComponent* Component, bool bIsCancelable = true, bool bIsStopMovement = false);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void EndMainAction(bool bIsStopMovement);
	void EndMainAction(UActionExecutor* OldExecutor, bool bIsStopMovement);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void ReservationEnqueue(const FActionReservator& Reservation);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void ReservationClear();

	UFUNCTION(BlueprintCallable, Category = "Action")
	void ReservationNext();


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	void NotifyExecutorEnded(UActionExecutor* EndExecutor, UUnitActionComponent* EndComponent);
	void NotifyExecutorEnded_Implementation(UActionExecutor* EndExecutor, UUnitActionComponent* EndComponent);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation")
	void ClaimPlayMontage(const FMontageEventInfo& MontageEvent);
	void ClaimPlayMontage_Implementation(const FMontageEventInfo& MontageEvent);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation")
	void NotifyMontageNodePassed(UActionExecutor* MontageExecutor, int RequestedID);
	void NotifyMontageNodePassed_Implementation(UActionExecutor* MontageExecutor, int RequestedID);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation")
	void ClaimStopMontage(UAnimMontage* WantMontage);
	void ClaimStopMontage_Implementation(UAnimMontage* WantMontage);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Movement")
	void ClaimStartMovement(const FVector& Destination, AActor* TargetActor, float AcceptanceRadius, UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID);
	void ClaimStartMovement_Implementation(const FVector& Destination, AActor* TargetActor, float AcceptanceRadius, UActionExecutor* Executor, UUnitActionComponent* TargetComponent, int ID);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Movement")
	void ClaimStopMovement();
	void ClaimStopMovement_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Movement")
	bool ClaimJump();
	bool ClaimJump_Implementation();

	UFUNCTION()
	void MontageStarted(UAnimMontage* Montage);

	UFUNCTION()
	void MontageEnded(UAnimMontage* Montage, bool bIsInterrupted);

	UFUNCTION()
	void MontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);
	UFUNCTION()
	void MontageNotifyEnd(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Unit")
	void Die();
	virtual void Die_Implementation();


public:
	bool IsSelectable_Implementation(class AOperator* Operator) { return true; }
	FSlateBrush GetSelectedIcon_Implementation() { return SelectedIcon; }
};
