// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/InfoConnectable.h"
#include "UnitComponentBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnComponentRemoved, UUnitComponentBase*, TargetComponent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnComponentMessage_Simple, UUnitComponentBase*, From, const FName&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnComponentMessage_Detail, UUnitComponentBase*, From, const FName&, Message, const FName&, Context);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnComponentMessage_Montage, UUnitComponentBase*, From, UAnimMontage*, Montage, bool, bIsStart, bool, bIsInterrupted);

class AOperator;
class UWidget;
class UUnitMainComponent;

UCLASS( Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WHITECLADWARRIORS_API UUnitComponentBase : public UActorComponent, public IInfoConnectable
{
	GENERATED_BODY()


public:
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Select")
	FOnComponentRemoved OnComponentRemoved;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Unit")
	FOnComponentMessage_Simple OnComponentMessage_Simple;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Unit")
	FOnComponentMessage_Detail OnComponentMessage_Detail;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Unit")
	FOnComponentMessage_Montage OnComponentMessage_Montage;

	UPROPERTY(BlueprintReadOnly, Category = "UnitComponent")
	UUnitMainComponent* OwnerUnit;


public:
	virtual void BeginDestroy() override;

public:
	UFUNCTION(BlueprintCallable, Category = "UnitComponent")
	void BroadcastMessage_Removed();

	UFUNCTION(BlueprintCallable, Category = "UnitComponent")
	void BroadcastMessage_Simple(const FName& Message);

	UFUNCTION(BlueprintCallable, Category = "UnitComponent")
	void BroadcastMessage_Detail(const FName& Message, const FName& Context);

	UFUNCTION(BlueprintCallable, Category = "UnitComponent")
	void BroadcastMessage_Montage(UAnimMontage* Montage, bool bIsStart, bool bIsInterrupted);

	UFUNCTION(BlueprintCallable, Category = "UnitComponent")
	void ReceiveUnitMessage_Simple(const FName& Message);

	UFUNCTION(BlueprintCallable, Category = "UnitComponent")
	void ReceiveUnitMessage_Detail(const FName& Message, const FName& Context);

	UFUNCTION(BlueprintCallable, Category = "UnitComponent")
	void ReceiveUnitMessage_Montage(UAnimMontage* Montage, bool bIsStart, bool bIsInterrupted);

	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Variable")
	FVector GetLocation();
	FVector GetLocation_Implementation();

	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Variable")
	FVector GetDirection(FVector Destination, bool bIsIgnoreZ = false);
	FVector GetDirection_Implementation(FVector Destination, bool bIsIgnoreZ = false);

	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Unit")
	UUnitMainComponent* GetOwnerUnit() const;
	inline UUnitMainComponent* GetOwnerUnit_Implementation() const { return OwnerUnit; }

	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Unit")
	UUnitMainComponent* SetOwnerUnit(UUnitMainComponent* NewUnit);
	inline UUnitMainComponent* SetOwnerUnit_Implementation(UUnitMainComponent* NewUnit);

	UFUNCTION(BlueprintCallable, Category = "Variable", meta = (ExpandEnumAsExecs = "ReturnValue"))
	bool TryGetOwnerUnit(UUnitMainComponent*& ResultUnit) const;

};
