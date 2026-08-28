// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/Selectables/Components/UnitActionComponent.h"
#include "Generals/Structs/DamageStructures.h"
#include "Generals/Structs/InputPackage.h"
#include "Generals/Structs/ActionStructures.h"
#include "Generals/Structs/MontageStructures.h"
#include "GameFramework/Character.h"
#include "Styling/SlateBrush.h"
#include "Interfaces/Damageable.h"
#include "Interfaces/Selectable.h"
#include "Interfaces/Stoppable.h"
#include "Interfaces/InfoConnectable.h"
#include "Interfaces/PlayerConnectable.h"
#include "UnitMainComponent.generated.h"

class AActionBase;
class AOperator;
class AIngameController;
class UActionTargetContainer;
class UActionExecutor;
class UActionNode;
class UFillableValue;
class UValueContainer;

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
	bool SetEnd(int64 EndExecutorID, UUnitActionComponent* EndComponent);
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMainActionChanged, const FMainActionInfo&, NewMainAction, bool, bIsValid);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUnitDie, UUnitMainComponent*, TargetUnit, const FDamageInfo&, LastAttackDamageInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnitRevive);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUnitDamage, UUnitMainComponent*, TargetUnit, const FDamageInfo&, Info);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnMovementStart, const FVector&, Destination, AActor*, TargetActor, float, AcceptanceRadius, const FActionCursorFinder&, WantCursor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMovementStop);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnMontageNotify, const FActionCursorFinder&, Cursor, FName, NotifyName);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnMontageStart, const FActionCursorFinder&, Cursor);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnMontageEnd, const FActionCursorFinder&, Cursor, bool, bIsInterrupted);

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
class WHITECLADWARRIORS_API UUnitMainComponent : public UUnitActionComponent, public ISelectable, public IStoppable, public IDamageable, public IPlayerConnectable
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Select")
	FOnUnitDamage OnUnitDamage;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Select")
	FOnSuccessDamage OnSuccessDamage;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Select")
	FOnUnitDie OnUnitDie;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Select")
	FOnUnitRevive OnUnitRevive;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Select")
	FOnMovementStop OnMovementStop;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Select")
	FOnMovementStart OnMovementStart;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Select")
	FOnMainActionChanged OnMainActionChanged;



protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Select")
	FSlateBrush SelectedIcon;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Select")
	FText SelectedName;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Hero")
	FText ClassName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	TMap<FName, FName> TaggedActionMap;

	UPROPERTY(BlueprintReadOnly, Category = "ValueMap");
	TObjectPtr<UValueContainer> ValueMap;

	UPROPERTY(BlueprintReadOnly, Category = "ValueMap")
	TMap<AActionBase*, FActionTargetContainer>  ActionMap;

	UPROPERTY(BlueprintReadOnly, Category = "Select")
	TMap<UUnitMainComponent*, float> DamageMap;

	TQueue<FActionReservator> ActionQueue;

	FActionReservator CurrentReservatedAction;

	UPROPERTY(BlueprintReadOnly, Category = "Select")
	TArray<UUnitComponentBase*> UnitComponentArray;

	UPROPERTY(BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess = true))
	TObjectPtr<AIngameController> PlayerController;

	UPROPERTY(BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> Mesh;

	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	TObjectPtr<UAnimInstance> AnimInstance;

	UPROPERTY(BlueprintReadOnly, Category = "MainAction", meta = (AllowPrivateAccess = true))
	FMainActionInfo MainAction;

	UPROPERTY(BlueprintReadOnly, Category = "Intention", meta = (AllowPrivateAccess = true))
	FActionIntentContainer CurrentIntention;

	FMontageEventInfo InputReadyMontageEvent;
	FMontageEventInfo MainActionMontageEvent;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UnitComponent", meta = (AllowPrivateAccess = true))
	EUnitControlledType ControlledType;

	UPROPERTY(BlueprintreadOnly, Category = "Damage")
	float TotalTakeDamage = 0;

	bool bIsDie = false;

public:
	UUnitMainComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

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

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Widget")
	TArray<UOrderedGenericWidgetClaim*> GetUnitInfoWidget(EInfoWidgetType WantType) const;
	TArray<UOrderedGenericWidgetClaim*> GetUnitInfoWidget_Implementation(EInfoWidgetType WantType) { return TArray<UOrderedGenericWidgetClaim*>(); }

	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Variable")
	bool HasOperatorAuthority(AOperator* From) const;
	bool HasOperatorAuthority_Implementation(AOperator* From) const;


	UFUNCTION(BlueprintPure, Category = "Action")
	FMainActionInfo GetMainActionInfo() const;

	UFUNCTION(BlueprintPure, Category = "Action")
	bool HasMainAction() const;

	UFUNCTION(BlueprintPure, Category = "Animation")
	bool HasInputReadyMontage() const;

	UFUNCTION(BlueprintPure, Category = "Animation")
	FMontageEventInfo GetActionMontageInfo() const;

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

	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Focus")
	bool GetFocusLocation(FVector& OutResult) const;
	bool GetFocusLocation_Implementation(FVector& OutResult) const;

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

	UFUNCTION(BlueprintCallable, Category = "Action")
	void AddAction(UUnitActionComponent* From, AActionBase* WantAction);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void AddActionFromName(UUnitActionComponent* From, const FName& WantName);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void AddActionFromNameList(UUnitActionComponent* From, TArray<FName> ActionNameList);

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Action")
	bool GetMainActionExecutable();
	virtual bool GetMainActionExecutable_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Action")
	bool GetActionExecutable();
	virtual bool GetActionExecutable_Implementation();

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool SetMainAction(const FMainActionInfo& Info, UActionNode* WantNode);
	bool SetMainAction(const FActionCursorFinder& WantCursor, UActionNode* WantNode);

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool StopMainAction();

	UFUNCTION(BlueprintCallable, Category = "Action") 
	void ReservationEnqueue(const FActionReservator& Reservation);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void ReservationClear();

	UFUNCTION(BlueprintCallable, Category = "Action")
	void ReservationNext();


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	void NotifyExecutorEnded(int64 EndExecutorID, UUnitActionComponent* EndComponent);
	void NotifyExecutorEnded_Implementation(int64 EndExecutorID, UUnitActionComponent* EndComponent);

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
	void Die(const FDamageInfo& LastAttackDamageInfo);
	virtual void Die_Implementation(const FDamageInfo& LastAttackDamageInfo);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Unit")
	void OnDie(const FDamageInfo& LastAttackDamageInfo, const TMap<UUnitMainComponent*, float>& LastDamageMap, float LastTotalDamage);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Unit")
	void Revive();
	virtual void Revive_Implementation();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Unit")
	void OnRevive();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Unit")
	void OnTakeDamage(const FDamageInfo& AttackDamageInfo);

public:
	virtual bool GetMainActionCancelable() const override;
	virtual bool TrySetMainAction_Implementation(const FActionCursorFinder& WantCursor, UActionNode* TargetNode) override;
	virtual void EndMainAction(int64 OldExecutorID, UUnitActionComponent* OldComponent) override;
	virtual void OnInputStart_Implementation(const FInputClaim& StartedInput) override;
	virtual void OnInputEnd_Implementation(const FInputClaim& EndedInput) override;

	virtual bool IsSelectable_Implementation(AOperator* Operator) { return true; }
	virtual FSlateBrush GetSelectedIcon_Implementation() { return SelectedIcon; }
	virtual FText GetSelectedName_Implementation() { return SelectedName; }
	virtual TArray<UOrderedGenericWidgetClaim*> GetInfoWidget_Implementation(EInfoWidgetType WantType, AOperator* Operator) const;

	virtual void SetStop_Implementation() override;

	virtual float TakeDamage_Implementation(const FDamageInfo& Info, bool& bIsKill);
	virtual bool GetIsAttackable_Implementation(UUnitMainComponent* From);
	virtual bool GetIsDamageable_Implementation(UUnitMainComponent* From);
	virtual bool GetIsDie_Implementation();
	virtual AOperator* GetOperator_Implementation() const override;

	virtual void OnPlayerConnected_Implementation(AIngameController* NewPlayer);
	virtual void OnPlayerDisconnected_Implementation(AIngameController* OldPlayer);
	virtual AIngameController* GetConnectedPlayerController_Implementation() { return PlayerController; }


public:
	UFUNCTION(BlueprintCallable, Category = "Unit")
	static UUnitMainComponent* GetUnit(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Unit", meta = (ExpandEnumAsExecs = "ReturnValue"))
	static bool TryGetUnit(AActor* Target, UUnitMainComponent*& OutResult);

};
