// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/Selectables/Components/UnitComponentBase.h"
#include "Generals/Structs/DamageStructures.h"
#include "Generals/Structs/InputPackage.h"
#include "Generals/Structs/ActionStructures.h"
#include "Generals/Structs/MontageStructures.h"
#include "GameFramework/Character.h"
#include "Styling/SlateBrush.h"
#include "Interfaces/Damageable.h"
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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUnitDamage, UUnitMainComponent*, TargetUnit, const FDamageInfo&, Info);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnMovementStart, const FVector&, Destination, AActor*, TargetActor, float, AcceptanceRadius, const FActionCursorFinder&, WantCursor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackTargetChanged, AActor*, TargetActor);

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
class WHITECLADWARRIORS_API UUnitMainComponent : public UUnitComponentBase, public ISelectable, public IDamageable, public IPlayerConnectable
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "FillValue")
	FOnFillValueAdded OnFillValueAdded;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "FillValue")
	FOnFillValueRemoved OnFillValueRemoved;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Select")
	FOnUnitDamage OnUnitDamage;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Select")
	FOnUnitDie OnUnitDie;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Select")
	FOnMovementStop OnMovementStop;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Select")
	FOnMovementStart OnMovementStart;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Select")
	FOnAttackTargetChanged OnAttackTargetChanged;


protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Select")
	FSlateBrush SelectedIcon;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Select")
	FText SelectedName;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Hero")
	FText ClassName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	TMap<FName, FName> TaggedActionMap;

	TMap<AActionBase*, FActionTargetContainer>  ActionMap;
	TMap<FName, UFillableValueComponent*> FillValueMap;
	TMap<UUnitMainComponent*, float> DamageMap;

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
	FMontageEventInfo MainActionMontageEvent;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UnitComponent", meta = (AllowPrivateAccess = true))
	EUnitControlledType ControlledType;

	UPROPERTY(BlueprintreadOnly, Category = "Damage")
	float TotalTakeDamage = 0;

	bool bIsDie = false;


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
	bool HasOperatorAuthority(AOperator* From) const;
	bool HasOperatorAuthority_Implementation(AOperator* From) const;

	UFUNCTION(BlueprintPure, Category = "Action")
	bool HasMainAction() const;

	UFUNCTION(BlueprintPure, Category = "Animation")
	bool HasInputReadyMontage() const;

	UFUNCTION(BlueprintPure, Category = "Animation")
	bool HasActionMontage() const;

	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Variable")
	bool IsAlly(EUnitControlledType OtherType) const;
	bool IsAlly_Implementation(EUnitControlledType OtherType) const;

	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Variable")
	EUnitAllyType GetAllyTypeFromOperator(AOperator* From);
	EUnitAllyType GetAllyTypeFromOperator_Implementation(AOperator* From) const;

	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Variable")
	EUnitAllyType GetAllyTypeFromUnit(UUnitMainComponent* From);
	EUnitAllyType GetAllyTypeFromUnit_Implementation(UUnitMainComponent* From) const;


	UFUNCTION(BlueprintPure, Category = "Action")
	TArray<AActionBase*> GetActionList() const;

	UFUNCTION(BlueprintPure, Category = "Action")
	TArray<AActionBase*> GetActionFromKey(FKey WantKey) const;

	UFUNCTION(BlueprintPure, Category = "Action")
	FName GetActionFromTag(FName WantTag) const;

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
	bool StopMainAction() { return SetMainAction(FActionCursorFinder::None); }

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
	bool PlayInputReadyMontage(const FMontageEventInfo& MontageEvent);
	bool PlayInputReadyMontage_Implementation(const FMontageEventInfo& MontageEvent);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation")
	void StopInputReadyMontage();
	void StopInputReadyMontage_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation")
	bool PlayMainActionMontage(const FMontageEventInfo& MontageEvent);
	bool PlayMainActionMontage_Implementation(const FMontageEventInfo& MontageEvent);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation")
	bool StopMainActionMontage(bool bIsInterrupted);
	bool StopMainActionMontage_Implementation(bool bIsInterrupted);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Movement")
	bool ClaimStartMovement(const FVector& Destination, AActor* TargetActor, float AcceptanceRadius, const FActionCursorFinder& WantCursor);
	bool ClaimStartMovement_Implementation(const FVector& Destination, AActor* TargetActor, float AcceptanceRadius, const FActionCursorFinder& WantCursor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Movement")
	bool ClaimStopMovement();
	bool ClaimStopMovement_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Movement")
	bool ClaimAttackTarget(AActor* TargetActor);
	bool ClaimAttackTarget_Implementation(AActor* TargetActor);

	float* GetDamageReference(UUnitMainComponent* From);

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Damage")
	float GetDamageValue(UUnitMainComponent* From);
	float GetDamageValue_Implementation(UUnitMainComponent* From);

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Damage")
	float GetDamagePercent(UUnitMainComponent* From);
	float GetDamagePercent_Implementation(UUnitMainComponent* From);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
	float AddDamageValue(UUnitMainComponent* From, float Value);
	float AddDamageValue_Implementation(UUnitMainComponent* From, float Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
	void ResetDamageFrom(UUnitMainComponent* From);
	void ResetDamageFrom_Implementation(UUnitMainComponent* From);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
	void ResetDamageValue();
	void ResetDamageValue_Implementation();

	UFUNCTION(BlueprintCallable, Category = "UnitComponent")
	void UnitMessage_Simple(const FName& Message);

	UFUNCTION(BlueprintCallable, Category = "UnitComponent")
	void UnitMessage_Detail(const FName& Message, const FName& Context);

	UFUNCTION(BlueprintCallable, Category = "UnitComponent")
	void UnitMessage_Montage(UAnimMontage* Montage, bool bIsStart, bool bIsInterrupted);

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
	virtual bool IsSelectable_Implementation(AOperator* Operator) { return true; }
	virtual FSlateBrush GetSelectedIcon_Implementation() { return SelectedIcon; }
	virtual FText GetSelectedName_Implementation() { return SelectedName; }
	virtual TArray<UOrderedGenericWidgetClaim*> GetInfoWidget_Implementation(EInfoWidgetType WantType, AOperator* Operator) const;

	virtual float TakeDamage_Implementation(const FDamageInfo& Info, bool& bIsKill);
	virtual bool GetIsAttackable_Implementation(UUnitMainComponent* From);
	virtual bool GetIsDamageable_Implementation(UUnitMainComponent* From);
	virtual bool GetIsDie_Implementation();

	virtual void OnPlayerConnected_Implementation(AIngameController* NewPlayer);
	virtual void OnPlayerDisconnected_Implementation(AIngameController* OldPlayer);
	virtual AIngameController* GetConnectedPlayerController_Implementation() { return PlayerController; }

public:
	UFUNCTION(BlueprintCallable, Category = "Unit")
	static UUnitMainComponent* GetUnit(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Unit", meta = (ExpandEnumAsExecs = "ReturnValue"))
	static bool TryGetUnit(AActor* Target, UUnitMainComponent*& OutResult);

};
