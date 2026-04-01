// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GenericWidgetClaimer.generated.h"

class UGenericWidgetBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWidgetConnected, UGenericWidgetBase*, NewWidget);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnWidgetConnectedSingle, UGenericWidgetBase*, NewWidget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWidgetUpdated, UGenericWidgetBase*, TargetWidget);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnWidgetUpdatedSingle, UGenericWidgetBase*, TargetWidget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWidgetUpdateClaim, UGenericWidgetBase*, TargetWidget);

/**
 * 
 */
USTRUCT(BlueprintType)
struct FGenericWidgetClaimer
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	int UIOrder = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	FName ClaimWidgetType;
};

UCLASS(BlueprintType, Blueprintable)
class UGenericWidgetClaim : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Widget")
	FOnWidgetConnected OnWidgetConnected;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Widget")
	FOnWidgetUpdated OnWidgetUpdated;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Widget")
	FOnWidgetUpdateClaim OnWidgetUpdateClaim;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget", Meta = (ExposeOnSpawn = "true"))
	FName WidgetType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget", Meta = (ExposeOnSpawn = "true"))
	TArray<UGenericWidgetClaim*> Children;

public:
	UGenericWidgetClaim();

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Widget")
	void Initialize();

	UFUNCTION(BlueprintCallable, Category = "Widget")
	void AssignGenericWidgetEvents(const FOnWidgetConnectedSingle& Connected, const FOnWidgetUpdatedSingle& Updated);
};

UCLASS(BlueprintType)
class UOrderedGenericWidgetClaim : public UGenericWidgetClaim
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget", Meta = (ExposeOnSpawn = "true"))
	int UIOrder = 0;
};