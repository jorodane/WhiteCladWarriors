// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InfoConnectable.generated.h"

class UWidget;

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
	TArray<FGenericWidgetClaimer> GetInfoWidgets() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericWidget")
	TArray<FGenericWidgetClaimer> GetPortraitWidgets() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericWidget")
	void ConnectInfoWidget(UWidget* TargetWidget, FName ClaimedTag);

	UFUNCTION(BlueprintNativeEvent, Category = "GenericWidget")
	void OnConnectInfoWidget(UWidget* TargetWidget, FName ClaimedTag);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericWidget")
	void DisconnectInfoWidget(UWidget* TargetWidget, FName ClaimedTag);

	UFUNCTION(BlueprintNativeEvent, Category = "GenericWidget")
	void OnDisconnectInfoWidget(UWidget* TargetWidget, FName ClaimedTag);

};
