// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/InfoConnectable.h"
#include "UnitComponentBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnComponentRemoved, UUnitComponentBase*, TargetComponent);

class UWidget;

USTRUCT(BlueprintType)
struct FWidgetArray
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Widget")
	TArray<UWidget*> WidgetArray;
};

UCLASS( Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WHITECLADWARRIORS_API UUnitComponentBase : public UActorComponent, public IInfoConnectable
{
	GENERATED_BODY()


public:
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Select")
	FOnComponentRemoved OnComponentRemoved;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Select")
	TArray<FGenericWidgetClaimer> InfoWidgets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Select")
	TArray<FGenericWidgetClaimer> PortraitWidgets;

	UPROPERTY(BlueprintReadOnly, Category = "GenericWidget")
	TMap<FName, FWidgetArray> ConnectedWidgets;

public:
	virtual void BeginDestroy() override;

public:
	UFUNCTION(BlueprintPure, Category = "GenericWidget")
	TArray<UWidget*> GetConnectedWidgetsWithTag(FName WantTag);

	UFUNCTION(BlueprintCallable, Category = "UnitComponent")
	void BroadcastRemoveMessage();

public:
	TArray<FGenericWidgetClaimer> GetInfoWidgets_Implementation() const { return InfoWidgets; }
	TArray<FGenericWidgetClaimer> GetPortraitWidgets_Implementation() const { return PortraitWidgets; }
	void ConnectInfoWidget_Implementation(UWidget* TargetWidget, FName ClaimedTag);
	void DisconnectInfoWidget_Implementation(UWidget* TargetWidget, FName ClaimedTag);
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
