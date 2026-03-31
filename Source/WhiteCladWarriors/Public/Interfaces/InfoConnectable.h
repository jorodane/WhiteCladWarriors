// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InfoConnectable.generated.h"

class UWidget;

UENUM(BlueprintType)
enum class EInfoWidgetType : uint8
{
	Minimal, Hover, Hero, Detail, Inspector
};

USTRUCT(BlueprintType)
struct FGenericWidgetClaimer
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	int UIOrder = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	FName ClaimWidgetType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	FName ClaimWidgetTag;
};

UCLASS(BlueprintType)
class UGenericWidgetClaim : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget", Meta = (ExposeOnSpawn = "true"))
	FName WidgetType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget", Meta = (ExposeOnSpawn = "true"))
	FName WidgetTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget", Meta = (ExposeOnSpawn = "true"))
	TArray<UGenericWidgetClaim*> Children;
};

UCLASS(BlueprintType)
class UOrderedGenericWidgetClaim : public UGenericWidgetClaim
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget", Meta = (ExposeOnSpawn = "true"))
	int UIOrder = 0;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInfoConnectable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class WHITECLADWARRIORS_API IInfoConnectable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericWidget")
	TArray<FGenericWidgetClaimer> GetInfoWidgets(EInfoWidgetType WantType) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericWidget")
	void ConnectInfoWidget(EInfoWidgetType NewType, UWidget* TargetWidget, FName ClaimedTag);

	UFUNCTION(BlueprintNativeEvent, Category = "GenericWidget")
	void OnConnectInfoWidget(EInfoWidgetType NewType, UWidget* TargetWidget, FName ClaimedTag);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericWidget")
	void DisconnectInfoWidget(EInfoWidgetType OldType, UWidget* TargetWidget, FName ClaimedTag);

	UFUNCTION(BlueprintNativeEvent, Category = "GenericWidget")
	void OnDisconnectInfoWidget(EInfoWidgetType OldType, UWidget* TargetWidget, FName ClaimedTag);
};
