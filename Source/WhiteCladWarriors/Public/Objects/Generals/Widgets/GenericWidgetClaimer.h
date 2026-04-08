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

UENUM(BlueprintType)
enum class EWidgetTransformType : uint8
{
	AspectRatio, 
	Fill, Fill_Horizontal, Fill_Vertical,
	Size, HorizontalSize, VerticalSize, 
	HorizontalAlignment, VerticalAlignment, 
	Padding, Padding_Horizontal, Padding_Vertical, Padding_Left, Padding_Right, Padding_Top, Padding_Bottom
};

/**
 * 
 */
//USTRUCT(BlueprintType)
//struct FGenericWidgetClaimer
//{
//	GENERATED_BODY()
//
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget", Meta = (ExposeOnSpawn = "true"))
//	int UIOrder = 0;
//
//
//};

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
	TMap<EWidgetTransformType, float> WidgetTransform;

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