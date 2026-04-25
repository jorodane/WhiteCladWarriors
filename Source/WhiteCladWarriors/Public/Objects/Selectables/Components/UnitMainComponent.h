// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/Selectables/Components/UnitComponentBase.h"
#include "Generals/Structs/InputPackage.h"
#include "Generals/Structs/ActionStructures.h"
#include "Generals/Structs/MontageStructures.h"
#include "GameFramework/Character.h"
#include "Styling/SlateBrush.h"
#include "Interfaces/Selectable.h"
#include "Interfaces/InfoConnectable.h"
#include "Interfaces/PlayerConnectable.h"
#include "UnitMainComponent.generated.h"

class AActionBase;
class AOperator;
class AIngameController;
class UUnitActionComponent;
class UActionTargetContainer;
class UActionExecutor;
class UActionNode;
class UFillableValueComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFillValueAdded, UFillableValueComponent*, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFillValueRemoved, UFillableValueComponent*, Value);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnitDie, UUnitMainComponent*, TargetUnit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnMovementStart, const FVector&, Destination, AActor*, TargetActor, float, AcceptanceRadius, const FActionCursorFinder&, WantCursor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnitMessage_Simple, const FName&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUnitMessage_Detail, const FName&, Message, const FName&, Context);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnUnitMessage_Montage, const FName&, Message, UAnimMontage*, Montage, bool, bIsStart);

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

UENUM(BlueprintType)
enum class EUnitAllyType : uint8
{
	Normal, Own, Enemy, Ally
};

UENUM(BlueprintType)
enum class EUnitControlledType : uint8
{
	Neutral, Player, Monster
};

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class WHITECLADWARRIORS_API UUnitMainComponent : public UUnitComponentBase, public ISelectable, public IPlayerConnectable
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

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Select")
	FOnUnitMessage_Simple OnUnitMessage_Simple;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Select")
	FOnUnitMessage_Detail OnUnitMessage_Detail;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Select")
	FOnUnitMessage_Montage OnUnitMessage_Montage;


protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Select")
	FSlateBrush SelectedIcon;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Select")
	FText SelectedName;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Hero")
	FText ClassName;

	TMap<AActionBase*, FActionTargetContainer>  ActionMap;
	TMap<FName, UFillableValueComponent*> FillValueMap;

	TQueue<FActionReservator> ActionQueue;

	FActionReservator CurrentReservatedAction;

	UPROPERTY(BlueprintReadOnly, Category = "Select")
	TArray<UUnitComponentBase*> UnitComponentArray;

	UPROPERTY(BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess = true))
	TObjectPtr<AIngameController> PlayerController;

	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	TObjectPtr<UAnimInstance> AnimInstance;

	FMainActionInfo MainAction;

	FMontageEventInfo InputReadyMontageEvent;

	FMontageEventInfo ClaimedMontageEvent;
	FMontageEventInfo QueuedMontageEvent;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UnitComponent", meta = (AllowPrivateAccess = true))
	EUnitControlledType ControlledType;


protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintPure, Category = "Component")
	TArray<UUnitComponentBase*> GetComponents() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Mesh")
	USkeletalMeshComponent* GetMesh() const;
	USkeletalMeshComponent* GetMesh_Implementation() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mesh")
	USkeletalMeshComponent* SetMesh(USkeletalMeshComponent* NewMesh);
	USkeletalMeshComponent* SetMesh_Implementation(USkeletalMeshComponent* NewMesh);

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Mesh")
	float GetHalfHeight();
	float GetHalfHeight_Implementation() { return 0.0f; }

	UFUNCTION(BlueprintCallable, Category = "FillValue")
	bool AddFillValue(FName WantTag, UFillableValueComponent* Target);

	UFUNCTION(BlueprintCallable, Category = "FillValue")
	void RemoveFillValue(FName WantTag);

	UFUNCTION(BlueprintPure, Category = "FillValue")
	UFillableValueComponent* FindFillValue(FName WantTag);

	UFUNCTION(BlueprintCallable, Category = "FillValue")
	bool TryFindFillValue(FName WantTag, UFillableValueComponent*& Result);

	UFUNCTION(BlueprintPure, Category = "FillValue")
	TArray<UFillableValueComponent*> FindAllFillValue();

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Widget")
	TArray<UOrderedGenericWidgetClaim*> GetUnitInfoWidget(EInfoWidgetType WantType) const;
	TArray<UOrderedGenericWidgetClaim*> GetUnitInfoWidget_Implementation(EInfoWidgetType WantType) { return TArray<UOrderedGenericWidgetClaim*>(); }

	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Variable")
	bool HasOperatorAuthority(AOperator* From);
	bool HasOperatorAuthority_Implementation(AOperator* From);

	UFUNCTION(BlueprintPure, Category = "Action")
	bool HasMainAction();

	UFUNCTION(BlueprintPure, Category = "Animation")
	bool HasInputReadyMontage();

	UFUNCTION(BlueprintPure, Category = "Animation")
	bool HasActionMontage();

	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Variable")
	bool IsAlly(EUnitControlledType OtherType);
	bool IsAlly_Implementation(EUnitControlledType OtherType);

	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Variable")
	EUnitAllyType GetAllyType(AOperator* From);
	EUnitAllyType GetAllyType_Implementation(AOperator* From);


	UFUNCTION(BlueprintPure, Category = "Action")
	TArray<AActionBase*> GetActionList() const;

	UFUNCTION(BlueprintPure, Category = "Action")
	TArray<AActionBase*> GetActionFromKey(FKey WantKey) const;


	UFUNCTION(BlueprintPure, Category = "Action")
	TArray<FActionTargetContainer> GetActionContainerFromKey(FKey WantKey) const;

	UFUNCTION(BlueprintPure, Category = "Action")
	TArray<UUnitActionComponent*> GetComponentsWithAction(AActionBase* TargetAction) const;

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool GetSimpleAction(const FInputPackage& CurrentInput, AActionBase*& OutAction, TArray<UUnitActionComponent*>& OutComponents) const;

	UFUNCTION(BlueprintCallable, Category = "Action")
	void AddUnitComponent(UUnitComponentBase* NewComponent);

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
	void NotifyMontageNodePassed(const FActionCursorFinder& WantCursor);
	void NotifyMontageNodePassed_Implementation(const FActionCursorFinder& WantCursor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation")
	bool PlayInputReadyMontage(const FMontageEventInfo& MontageEvent);
	bool PlayInputReadyMontage_Implementation(const FMontageEventInfo& MontageEvent);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation")
	void StopInputReadyMontage();
	void StopInputReadyMontage_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation")
	bool ClaimPlayMontage(const FMontageEventInfo& MontageEvent);
	bool ClaimPlayMontage_Implementation(const FMontageEventInfo& MontageEvent);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation")
	bool ClaimStopMontage(UAnimMontage* WantMontage);
	bool ClaimStopMontage_Implementation(UAnimMontage* WantMontage);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Movement")
	bool ClaimStartMovement(const FVector& Destination, AActor* TargetActor, float AcceptanceRadius, const FActionCursorFinder& WantCursor);
	bool ClaimStartMovement_Implementation(const FVector& Destination, AActor* TargetActor, float AcceptanceRadius, const FActionCursorFinder& WantCursor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Movement")
	bool ClaimStopMovement();
	bool ClaimStopMovement_Implementation();

	UFUNCTION(BlueprintCallable, Category = "UnitComponent")
	void UnitMessage(const FName& Message);

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
	bool IsSelectable_Implementation(AOperator* Operator) { return true; }
	FSlateBrush GetSelectedIcon_Implementation() { return SelectedIcon; }
	FText GetSelectedName_Implementation() { return SelectedName; }
	TArray<UOrderedGenericWidgetClaim*> GetInfoWidget_Implementation(EInfoWidgetType WantType, AOperator* Operator) const;
	virtual void OnPlayerConnected_Implementation(AIngameController* NewPlayer);
	virtual void OnPlayerDisconnected_Implementation(AIngameController* OldPlayer);
	virtual AIngameController* GetConnectedPlayerController_Implementation() { return PlayerController; }
};
