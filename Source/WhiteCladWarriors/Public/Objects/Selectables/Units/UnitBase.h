// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Generals/Structs/InputPackage.h"
#include "Generals/Structs/ActionStructures.h"
#include "GameFramework/Character.h"
#include "Styling/SlateBrush.h"
#include "Interfaces/Selectable.h"
#include "UnitBase.generated.h"

class AActionBase;
class UUnitActionComponent;
class UActionExecutor;
class UActionNode;
class UFillValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFillValueAdded, UFillValue*, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFillValueRemoved, UFillValue*, Value);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnitDie, AUnitBase*, TargetUnit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnMovementStart, const FVector&, Destination, AActor*, TargetActor, float, AcceptanceRadius, const FActionCursorFinder&, WantCursor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMovementStop);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnMontageNotify, const FActionCursorFinder&, Cursor, FName, NotifyName);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnMontageStart, const FActionCursorFinder&, Cursor);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnMontageEnd, const FActionCursorFinder&, Cursor, bool, bIsInterrupted);

USTRUCT(BlueprintType)
struct FActionReservator
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	FActionCursorFinder Cursor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Input")
	FInputPackage Input;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	TArray<UUnitActionComponent*> RunningComponents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Valid")
	bool bIsValid = false;

	FActionReservator() {};
	FActionReservator(AOperator* TargetOperator, AActionBase* TargetAction, const FInputPackage& TargetInput)
	{
		Cursor.CurrentOperator = TargetOperator;
		Cursor.CurrentAction = TargetAction;
		Input = TargetInput;
	}

	void Clear();
	bool CheckValid();
	bool Run(TArray<UUnitActionComponent*> StartComponents);
	bool SetEnd(UActionExecutor* EndExecutor, UUnitActionComponent* EndComponent);


};

USTRUCT(BlueprintType)
struct FMontageEventInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	FActionCursorFinder Cursor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	TObjectPtr<UAnimMontage> MontageToPlay = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	float PlayRate = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	float StartingPosition = 0.0f;

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
	void Clear() { Cursor.Clear(); bIsStarted = false;}
	void MontageNotifyBegin(FName NotifyName);
	void MontageNotifyEnd(FName NotifyName);
	void MontageStart();
	void MontageEnd(bool bIsInterrupted);
};

USTRUCT(BlueprintType)
struct FMainActionInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	FActionCursorFinder Cursor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	bool bIsCancelable = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	bool bIsStopMovement = false;

	void Clear();

	void Set(const FActionCursorFinder& WantCursor, bool bWantIsCancelable = true, bool bWantIsStopMovement = false);

	void Clear(const UActionExecutor* OldExecutor);

	void SetActionMessage_Simple(FName Message);

	bool Cancel(bool bWantStopMovement);

	void End(bool bWantStopMovement);

	bool CheckValid() const;
};

UCLASS()
class WHITECLADWARRIORS_API AUnitBase : public ACharacter, public ISelectable
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "FillValue")
	FOnFillValueAdded OnFillValueAdded;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "FillValue")
	FOnFillValueRemoved OnFillValueRemoved;

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
	TMap<FName, UFillValue*> FillValueMap;

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
	UFUNCTION(BlueprintCallable, Category = "FillValue")
	UFillValue* AddFillValue(FName WantTag);

	UFUNCTION(BlueprintCallable, Category = "FillValue")
	void RemoveFillValue(FName WantTag);

	UFUNCTION(BlueprintPure, Category = "FillValue")
	UFillValue* FindFillValue(FName WantTag);

	UFUNCTION(BlueprintCallable, Category = "FillValue")
	bool TryFindFillValue(FName WantTag, UFillValue*& Result);

	UFUNCTION(BlueprintPure, Category = "FillValue")
	TArray<UFillValue*> FindAllFillValue();

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
	bool SetMainAction(const FActionCursorFinder& WantCursor, bool bIsCancelable = true, bool bIsStopMovement = false);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void EndMainAction(UActionExecutor* OldExecutor, UUnitActionComponent* OldComponent, bool bIsStopMovement);

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
	void NotifyMontageNodePassed(const FActionCursorFinder& WantCursor);
	void NotifyMontageNodePassed_Implementation(const FActionCursorFinder& WantCursor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation")
	void ClaimStopMontage(UAnimMontage* WantMontage);
	void ClaimStopMontage_Implementation(UAnimMontage* WantMontage);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Movement")
	void ClaimStartMovement(const FVector& Destination, AActor* TargetActor, float AcceptanceRadius, const FActionCursorFinder& WantCursor);
	void ClaimStartMovement_Implementation(const FVector& Destination, AActor* TargetActor, float AcceptanceRadius, const FActionCursorFinder& WantCursor);

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
