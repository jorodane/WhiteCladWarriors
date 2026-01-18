// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnMovementStart, const FVector&, Destination, AActor*, TargetActor, float, AcceptanceRadius, UActionExecutor*, Executor, int, ID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMovementStop);

DECLARE_DYNAMIC_DELEGATE_FourParams(FOnMontageNotify, UActionExecutor*, Executor, UUnitActionComponent*, TargetComponent, int, ID, FName, NotifyName);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnMontageStart, UActionExecutor*, Executor, UUnitActionComponent*, TargetComponent, int, ID);
DECLARE_DYNAMIC_DELEGATE_FourParams(FOnMontageEnd, UActionExecutor*, Executor, UUnitActionComponent*, TargetComponent, int, ID, bool, bIsInterrupted);

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

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation")
	void ClaimPlayMontage(const FMontageEventInfo& MontageEvent);
	void ClaimPlayMontage_Implementation(const FMontageEventInfo& MontageEvent);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation")
	void ClaimStopMontage(UAnimMontage* WantMontage);
	void ClaimStopMontage_Implementation(UAnimMontage* WantMontage);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Movement")
	void ClaimStartMovement(const FVector& Destination, AActor* TargetActor, float AcceptanceRadius, UActionExecutor* Executor, int ID);
	void ClaimStartMovement_Implementation(const FVector& Destination, AActor* TargetActor, float AcceptanceRadius, UActionExecutor* Executor, int ID);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Movement")
	void ClaimStopMovement();
	void ClaimStopMovement_Implementation();

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
