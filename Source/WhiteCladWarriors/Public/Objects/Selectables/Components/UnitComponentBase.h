// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/InfoConnectable.h"
#include "UnitComponentBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnComponentRemoved, UUnitComponentBase*, TargetComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnComponentMessage_Simple, UUnitComponentBase*, From, FName, Message);

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
	void ReceiveUnitMessage_Simple(const FName& Message);

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

//
//public:	
//	// Sets default values for this component's properties
//	UUnitComponentBase();
//
//protected:
//	// Called when the game starts
//	virtual void BeginPlay() override;
//
//public:	
//	// Called every frame
//	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
//
//		
};
